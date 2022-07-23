#include "fb.h"
#include "common.h"
#include "wiring.h"
#include "secrets.h"
#include "config.h"
#include "ui.h"

#include <Firebase_ESP_Client.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

namespace fb {
    FirebaseData fbdo;
    FirebaseData stream_data;
    FirebaseAuth auth;
    FirebaseConfig config;

    bool init_success = true;
    
    bool disp_data_updated = false;
    uint8_t disp_data[DISP_HEIGHT * 8][DISP_WIDTH];

    void token_status_cb(TokenInfo info) {
        if (info.status == token_status_error) {
            init_success = false;
            DEBUG_OUT_FP(PSTR("Token error: code: %d, message: %s\n"), info.error.code, info.error.message.c_str());
        }
    }
    
    void stream_timeout_cb(bool timeout) {
        if (timeout) {
            DEBUG_OUT_LN(F("Stream timed out!"));
        }
        if (!stream_data.httpConnected()) {
            DEBUG_OUT_FP(PSTR("Stream not connected: code: %d, message: %s\n"), stream_data.httpCode(), stream_data.errorReason().c_str());
        }
    }

    void stream_cb(FirebaseStream data) {
        if (data.payloadLength() > 2048) {
            DEBUG_OUT_FP(PSTR("Error: Unexpectedly long payload length: %d"), data.payloadLength());
            return;
        }
        if (data.dataTypeEnum() != fb_esp_rtdb_data_type_blob) {
            DEBUG_OUT_FP(PSTR("Error: Got unexpected data type: %d (expecting %d)"), data.dataTypeEnum(), fb_esp_rtdb_data_type_blob);
            return;
        }

        auto *display_data = data.to<std::vector<uint8_t>*>();
        uint16_t row = 0;
        uint16_t col = 0;
        for (uint8_t b : *display_data) {
            disp_data[row][col++] = b;
            if (col % DISP_WIDTH == 0) {
                col = 0;
                row++;
            }
        }
        // TODO: Verify data length
        disp_data_updated = true;
        DEBUG_OUT_LN(F("Got new data"));
    }

    bool init() {
        config.api_key = FIREBASE_API_KEY;
        config.database_url = FIREBASE_DB_URL;
        auth.user.email = config::global_config.db_auth_email;
        auth.user.password = config::global_config.db_auth_password;
        config.token_status_callback = token_status_cb;

        init_success = true;
        Firebase.begin(&config, &auth);
        // Firebase.begin() could fail if credentials are wrong
        if (!init_success) {
            return false;
        }
        Firebase.reconnectWiFi(true);
        fbdo.setBSSLBufferSize(2048, 512);
        stream_data.setBSSLBufferSize(512, 512);
        return true;
    }

    bool start_stream() {
        char path[64];
        snprintf_P(path, sizeof(path), PSTR("/data/%s/displayData"), config::global_config.db_data_location);
        if (!Firebase.RTDB.beginStream(&stream_data, path)) {
            DEBUG_OUT(F("Can't start stream: "));
            DEBUG_OUT_LN(stream_data.errorReason());
            return false;
        }
        Firebase.RTDB.setStreamCallback(&stream_data, stream_cb, stream_timeout_cb);
        return true;
    }

    bool check_ota() {
        DEBUG_OUT_LN(F("Checking for OTA updates..."));
        if (Firebase.RTDB.getBool(&fbdo, F("/ota/valid"))) {
            if (!fbdo.to<bool>()) {
                DEBUG_OUT_LN(F("/ota/valid not true, not updating"));
                return false;
            }
        }
        else {
            DEBUG_OUT(F("Failed to get /ota/valid (not updating): "));
            DEBUG_OUT_LN(fbdo.errorReason());
            return false;
        }
        
        if (Firebase.RTDB.getString(&fbdo, F("/ota/version"))) {
            if (!strcmp(fbdo.to<const char *>(), FIRMWARE_VER)) {
                DEBUG_OUT_LN(F("Firmware version not updated, not updating"));
                return false;
            }
            DEBUG_OUT(F("New firmware verson: "));
            DEBUG_OUT_LN(fbdo.to<const char *>());
        }
        else {
            DEBUG_OUT(F("Failed to get /ota/version (not updating): "));
            DEBUG_OUT_LN(fbdo.errorReason());
            return false;
        }

        
        return true;
    }

    bool ota_update() {
        if (Firebase.RTDB.endStream(&stream_data)) {
            DEBUG_OUT_LN(F("Ended display data stream for OTA update"));
        }
        else {
            DEBUG_OUT_LN(F("Error: Can't stop display data stream for OTA update (continuing with update)"));
        }

        if (!Firebase.RTDB.getInt(&fbdo, F("/ota/firmware/chunkCount"))) {
            DEBUG_OUT(F("Failed to get new firmware chunk count: "));
            DEBUG_OUT_LN(fbdo.errorReason());
            return false;
        }
        unsigned int chunk_count = fbdo.to<int>();
        if (chunk_count < 1) {
            DEBUG_OUT_LN(F("New firmware invalid: chunk count < 1"));
            return false;
        }

        ui::set_text("0%", nullptr);
        ui::progress_bar.set_max_progress(chunk_count);
        ui::progress_bar.set_progress(0);
        ui::set_layout(ui::LayoutType::LOADING_PROGRESS_BAR);

        DEBUG_OUT_LN(F("Starting Firebase OTA"));
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
            return false;
        }

        char path[48];
        uint8_t retry_count = 0;
        for (unsigned int i = 0; i < chunk_count; i ++) {
            snprintf_P(path, sizeof(path), PSTR("/ota/firmware/bin/%d"), i);
            if (!Firebase.RTDB.getBlob(&fbdo, path)) {
                DEBUG_OUT_FP(PSTR("Failed to get chunk %d: %s\n"), i, fbdo.errorReason().c_str());
                // Retry
                // Seems to fail every 100 request for some reason, but delay and then retrying fixes it
                if (++retry_count > 5) {
                    DEBUG_OUT_LN(F("Max retries exceeded"));
                    return false;
                }
                else {
                    // Try the same chunk again
                    i--;
                    delay(1000);
                    continue;
                }
            }
            retry_count = 0;

            auto *blob = fbdo.to<std::vector<uint8_t>*>();
            if (Update.write(blob->data(), blob->size()) == blob->size()) {
                DEBUG_OUT_FP(PSTR("Wrote %d bytes (chunk %d/%d)\n"), blob->size(), i + 1, chunk_count);
                ui::progress_bar.set_progress(i + 1);
                char str[10];
                sprintf_P(str, PSTR("%d%%"), 100 * (i + 1) / chunk_count);
                ui::set_text(str, nullptr);
            }
            else {
                Update.printError(Serial);
                return false;
            }
        }

        DEBUG_OUT_LN(F("Wrote all chunks; ending update..."));
        if (!Update.end(true)) {
            Update.printError(Serial);
            return false;
        }
        // Set layout first to update position and width of object
        // So the text can fit without scrolling
        ui::set_layout(ui::LayoutType::TEXT);
        ui::set_text("Please", "wait");
        DEBUG_OUT_LN(F("Update successfully downloaded!"));
        return true;
    }
}

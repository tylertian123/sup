#include "fb.h"
#include "common.h"
#include "secrets.h"
#include "config.h"

#include <Firebase_ESP_Client.h>
#include <stdio.h>

namespace fb {
    FirebaseData stream_data;
    FirebaseAuth auth;
    FirebaseConfig config;

    bool init_success = true;

    void token_status_cb(TokenInfo info) {
        if (info.status == token_status_error) {
            init_success = false;
            DEBUG_OUT_FP(PSTR("Token error: code: %d, message: %s\n"), info.error.code, info.error.message.c_str());
        }
    }
    
    void stream_timeout_cb(bool timeout) {
        if (timeout) {
            DEBUG_OUT(F("Stream timed out!"));
        }
        if (!stream_data.httpConnected()) {
            DEBUG_OUT_FP(PSTR("Stream not connected: code: %d, message: %s\n"), stream_data.httpCode(), stream_data.errorReason().c_str());
        }
    }

    void stream_cb(FirebaseStream data) {
        DEBUG_OUT(F("Received data:"));
        DEBUG_OUT(data.to<String>());
        DEBUG_OUT(F("Payload length:"));
        DEBUG_OUT(data.payloadLength());
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
        return true;
    }

    bool start_stream() {
        stream_data.setBSSLBufferSize(2048, 512);
        char path[64];
        snprintf_P(path, sizeof(path), PSTR("/data/%s/testData"), config::global_config.db_data_location);
        if (!Firebase.RTDB.beginStream(&stream_data, path)) {
            DEBUG_OUT(F("Error:"));
            DEBUG_OUT(stream_data.errorReason());
            return false;
        }
        Firebase.RTDB.setStreamCallback(&stream_data, stream_cb, stream_timeout_cb);
        return true;
    }
}

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <stdio.h>
#include <string.h>
#include <osapi.h>

#include "common.h"
#include "wiring.h"
#include "ui.h"
#include "display.h"
#include "config.h"
#include "nw.h"
#include "fb.h"

/*
 * NOTE: secrets.h defines:
 * FIREBASE_DB_URL: Firebase RTDB URL
 * FIREBASE_API_KEY: Firebase API key
 * DEFAULT_SSID: Default access point SSID if configuration is invalid
 * DEFAULT_PASSWORD: Default access point password if configuration is invalid
 */
#include "secrets.h"

bool init_success = false;
unsigned long ota_check_time = 0;

const char* rst_reason_str(uint32_t reason) {
    switch (reason) {
    case REASON_DEFAULT_RST:
        return PSTR("Power on");
    case REASON_WDT_RST:
        return PSTR("Hardware watchdog");
    case REASON_EXCEPTION_RST:
        return PSTR("Exception");
    case REASON_SOFT_WDT_RST:
        return PSTR("Software watchdog");
    case REASON_SOFT_RESTART:
        return PSTR("Soft restart");
    case REASON_DEEP_SLEEP_AWAKE:
        return PSTR("Deep sleep awake");
    case REASON_EXT_SYS_RST:
        return PSTR("External reset");
    default:
        return PSTR("Unknown");
    }
}

bool rst_reason_is_error(uint32_t reason) {
    return reason == REASON_WDT_RST || reason == REASON_SOFT_WDT_RST || reason == REASON_EXCEPTION_RST;
}

os_timer_t timer;

void timer_cb(void *arg) {
    ui::poll();
}

void setup() {
    Serial.begin(115200);
    DEBUG_OUT_LN(F("\n\n======= Starting ======="));
    DEBUG_OUT(F("INFO: Firmware version: "));
    DEBUG_OUT_LN(FIRMWARE_VER);

    rst_info *info = system_get_rst_info();
    DEBUG_OUT(F("Cause of reset: "));
    DEBUG_OUT_LN(FPSTR(rst_reason_str(info->reason)));

    config::init();
    if (!config::load_config()) {
        DEBUG_OUT_LN(F("Failed to load config object, using default values"));
    }

    ui::init();
    ui::error_led.set(false);
    ui::status_led.set(false);

    if (rst_reason_is_error(info->reason)) {
        ui::error_led.set(true);
    }

    os_timer_setfn(&timer, timer_cb, nullptr);
    os_timer_arm(&timer, 10, true);

    delay(1000);

    if (ui::input2.down) {
        DEBUG_OUT_LN(F("Forcing AP mode"));
    }
    nw::wifi_connect(!ui::input2.down);

    DEBUG_OUT_FP(PSTR("Local IP address: %s\n"), WiFi.localIP().toString().c_str());

    ui::set_text(F("DB"), F("Connecting"));
    if(!fb::init()) {
        DEBUG_OUT_LN(F("Failed to connect to Firebase!"));
        ui::set_text(nullptr, F("Connection error"));
        ui::set_layout(ui::LayoutType::ERROR_TEXT);
        ui::error_led.set(true);
    }
    else {
        DEBUG_OUT_LN(F("Connected to Firebase"));
        ui::set_text(nullptr, F("Starting"));
        if (!fb::start_stream()) {
            DEBUG_OUT_LN(F("Failed to start stream!"));
            ui::set_text(nullptr, F("Stream error"));
            ui::set_layout(ui::LayoutType::ERROR_TEXT);
            ui::error_led.set(true);
        }
        else {
            ui::set_text(nullptr, F("Waiting for data"));
            DEBUG_OUT_LN(F("Stream started"));
            init_success = true;
        }
    }
}

void loop() {
    if (init_success) {
        // Make Firebase do its auth thing
        if (Firebase.ready() && millis() > ota_check_time) {
            // OTA check every 10 minutes
            ota_check_time += 1000 * 60 * 10;
            if (fb::check_ota()) {
                DEBUG_OUT_LN(F("OTA updating from Firebase..."));
                if (fb::ota_update()) {
                    delay(1000);
                    ESP.restart();
                }
                else {
                    ui::set_text(F("Error"), F("Update failed!"));
                    ui::set_layout(ui::LayoutType::ERROR_TEXT);
                    // TODO: Restart data stream?
                }
            }
        }
    }
    ui::poll();
}

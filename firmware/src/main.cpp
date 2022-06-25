#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "wiring.h"
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

display::Display<DISP_WIDTH, DISP_HEIGHT, DISP1_CS, DISP2_CS> disp;

void setup() {
    pinMode(INPUT_BTN, INPUT_PULLUP);
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, 0);

    Serial.begin(115200);
    DEBUG_OUT_LN(F("Started"));

    disp.init();

    config::init();
    if (!config::load_config()) {
        DEBUG_OUT_LN(F("Failed to load config object, using default values"));
    }

    if (!digitalRead(D1)) {
        DEBUG_OUT_LN(F("Forcing AP mode"));
    }

    nw::wifi_connect(digitalRead(D1));

    DEBUG_OUT_FP(PSTR("Local IP address: %s\n"), WiFi.localIP().toString().c_str());

    if(!fb::init()) {
        DEBUG_OUT_LN(F("Failed to connect to Firebase!"));
    }
    else {
        DEBUG_OUT_LN(F("Connected to Firebase"));
        if (!fb::start_stream()) {
            DEBUG_OUT_LN(F("Failed to start stream!"));
        }
        else {
            DEBUG_OUT_LN(F("Stream started"));
            init_success = true;
        }
    }
}

void loop() {
    if (init_success) {
        if (Firebase.ready()) {
            // Nothing to do right now
        }
        if (fb::disp_data_updated) {
            fb::disp_data_updated = false;
            memcpy(disp.disp_buf, fb::disp_data, sizeof(disp.disp_buf));
            disp.update();
            DEBUG_OUT_LN(F("Display updated"));
        }
    }
}

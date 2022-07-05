#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <stdio.h>
#include <string.h>

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

void timer_cb() {
    ui::poll(init_success);
}

void setup() {
    Serial.begin(115200);
    DEBUG_OUT_LN(F("Started"));


    ui::init();
    digitalWrite(STATUS_LED, 0);

    timer1_attachInterrupt(timer_cb);
    // 80MHz / 256 / 6250 = 20ms^-1
    timer1_write(6250);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);

    config::init();
    if (!config::load_config()) {
        DEBUG_OUT_LN(F("Failed to load config object, using default values"));
    }

    if (ui::input1.down) {
        DEBUG_OUT_LN(F("Forcing AP mode"));
    }
    nw::wifi_connect(!ui::input1.down);

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
        // Make Firebase do its auth thing
        // Data comes from a stream callback so no need to check this result here
        Firebase.ready();
    }
}

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

#include "nw.h"
#include "common.h"
#include "config.h"

/*
 * NOTE: secrets.h defines:
 * FIREBASE_DB_URL: Firebase RTDB URL
 * FIREBASE_API_KEY: Firebase API key
 * DEFAULT_SSID: Default access point SSID if configuration is invalid
 * DEFAULT_PASSWORD: Default access point password if configuration is invalid
 */
#include "secrets.h"


void setup() {
    pinMode(D1, INPUT_PULLUP);
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, 0);

    Serial.begin(115200);
    DEBUG_OUT(F("Started"));

    config::init();
    if (!config::load_config()) {
        DEBUG_OUT(F("Failed to load config object, using default values"));
    }

    if (!digitalRead(D1)) {
        DEBUG_OUT(F("Forcing AP mode"));
    }

    nw::wifi_connect(digitalRead(D1));

    DEBUG_OUT(WiFi.localIP());
}

void loop() {
    
}

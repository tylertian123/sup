#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "nw.h"
#include "common.h"


void setup() {
    pinMode(D1, INPUT_PULLUP);
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, 0);

    Serial.begin(115200);
    DEBUG_OUT("Started");
    if (!digitalRead(D1)) {
        DEBUG_OUT("Forcing AP mode");
    }

    nw::wifi_connect(digitalRead(D1));

    DEBUG_OUT(WiFi.localIP());
}

void loop() {
    
}

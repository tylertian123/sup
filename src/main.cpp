#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "nw.h"


void setup() {
    Serial.begin(115200);
    Serial.println("Started");

    nw::wifi_connect();

    Serial.println(WiFi.localIP());
}

void loop() {
    
}

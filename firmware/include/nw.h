#pragma once

#include <ESP8266WebServer.h>

namespace nw {
    void enterprise_connect(const char *ssid, const char *username, const char *identity, const char *password);
    void decode_wifi_status(int status, char *buf);
    void set_country();
    void wifi_connect(bool use_saved = true);
    void init_ap();
    void init_server(ESP8266WebServer &server);
}

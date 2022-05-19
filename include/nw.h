#pragma once

#include <ESP8266WebServer.h>

namespace nw {
    void wifi_connect();
    void init_ap();
    void init_server(ESP8266WebServer &server);
}

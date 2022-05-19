#include "nw.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace nw {

    const char *hostname = "esp8266";
    const unsigned long timeout = 60000UL;

    const IPAddress ip_local(192, 168, 0, 1);
    const IPAddress ip_gateway(192, 168, 0, 1);
    const IPAddress ip_subnet(255, 255, 255, 0);

    const char *ssid = hostname;
    const char *password = "testespproject";

    int connect_status = WL_DISCONNECTED;
    String input_ssid, input_password;

    void wifi_connect() {
        // Try to connect with saved network
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);
        WiFi.hostname(hostname);
        WiFi.begin();

        connect_status = WiFi.waitForConnectResult(timeout);
        if (connect_status == WL_CONNECTED) {
            return;
        }

        // In case of failure, run the access point + config server
        while (connect_status != WL_CONNECTED) {
            // Enable access point
            init_ap();
            {
                // Run the server until we have the SSID and password
                ESP8266WebServer server(80);
                init_server(server);
                server.begin();
                while (connect_status != -1) {
                    server.handleClient();
                }
            }
            // This delay seems to be necessary, otherwise the last page will time out
            delay(2000);
            WiFi.softAPdisconnect(true);

            // Try connecting
            WiFi.persistent(true);
            WiFi.begin(input_ssid, input_password);
            connect_status = WiFi.waitForConnectResult(timeout);
            WiFi.persistent(false);
        }
    }

    void init_ap() {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(ip_local, ip_gateway, ip_subnet);
        WiFi.softAP(ssid, password);
    }

    void init_server(ESP8266WebServer &server) {
        // For debug
        server.on("/status", HTTP_GET, [&server]() {
            char resp[] = "Status:  ";
            resp[8] = '0' + connect_status;
            server.send(200, "text/plain", resp);
        });
        // Main configuration UI
        server.on("/wifi", HTTP_GET, [&server]() {
            String html("<!DOCTYPE html><html><head><title>Configure Wi-Fi</title></head><body><h2>Configure Wi-Fi</h2>Current status: ");
            html.reserve(500);
            switch (connect_status) {
            case WL_NO_SSID_AVAIL:
                html += "Connect Failed (Invalid SSID)";
                break;
            case WL_CONNECTED:
                html += "Connected";
                break;
            case WL_CONNECT_FAILED:
                html += "Connect Failed";
                break;
            case WL_WRONG_PASSWORD:
                html += "Connect Failed (Wrong Password)";
                break;
            default:
                html += "Unknown (";
                html += connect_status;
                html += ")";
                break;
            }
            html += "<br><form method=\"post\" action=\"/wifi-connect\" enctype=\"application/x-www-form-urlencoded\"><label>Network Name (SSID):<br><input type=\"text\" name=\"ssid\" required></label><br><label>Password:<br><input type=\"password\" name=\"password\" required></label><br><input type=\"submit\" value=\"Connect\"></form></body></html>";

            server.send(200, "text/html", html);
        });
        // Config backend
        server.on("/wifi-connect", HTTP_POST, [&server]() {
            if (!server.hasArg("ssid") && server.hasArg("password")) {
                server.send(400, "text/plain", "Bad Request");
                return;
            }
            input_ssid = server.arg("ssid");
            input_password = server.arg("password");
            server.send_P(200, "text/html", PSTR(
                "<!DOCTYPE html><html><head><title>Connecting to Wi-Fi</title></head><body><p>Trying to connect to the network.</p><p>This server and access point will be offline during this process, and will restart if connection fails. You may have to manually disconnect and reconnect to this network to retry if connection fails.</p></body></html>"
            ));
            // Set this to indicate that we should reconnect now
            connect_status = -1;
        });
    }
}

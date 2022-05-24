#include "nw.h"
#include "common.h"
#include "config.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <stdio.h>

namespace nw {

    const char *hostname = "sup";
    const unsigned short timeout = 60;

    const IPAddress ip_local(192, 168, 0, 1);
    const IPAddress ip_gateway(192, 168, 0, 1);
    const IPAddress ip_subnet(255, 255, 255, 0);

    int connect_status = WL_DISCONNECTED;
    String input_ssid, input_password;

    void wifi_connect(bool use_saved) {
        // Try to connect with saved network
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);
        WiFi.hostname(hostname);

        connect_status = -2;
        if (use_saved) {
            WiFi.begin();

            int counter = 0;
            while ((connect_status = WiFi.status()) == WL_DISCONNECTED && counter * 2 < timeout) {
                delay(500);
                digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
                counter++;
            }
        }

        if (connect_status == WL_CONNECTED) {
            DEBUG_OUT(F("Connected to saved network"));
            digitalWrite(STATUS_LED, 1);
            return;
        }
        DEBUG_OUT(F("Can't connect to saved network"));


        // In case of failure, run the access point + config server
        while (connect_status != WL_CONNECTED) {
            digitalWrite(STATUS_LED, 0);

            // Enable access point
            init_ap();
            if (!MDNS.begin(hostname)) {
                DEBUG_OUT(F("Can't start mDNS"));
            }
            {
                // Run the server until we have the SSID and password
                ESP8266WebServer server(80);
                init_server(server);
                server.begin();
                DEBUG_OUT(F("Server is online"));
                while (connect_status != -1) {
                    MDNS.update();
                    server.handleClient();
                }
            }
            // This delay seems to be necessary, otherwise the last page will time out
            delay(2000);
            WiFi.softAPdisconnect(true);

            DEBUG_OUT(F("Connecting to SSID/Password:"));
            DEBUG_OUT(input_ssid);
            DEBUG_OUT(input_password);

            // Try connecting
            WiFi.persistent(true);
            WiFi.begin(input_ssid, input_password);
            int counter = 0;
            while ((connect_status = WiFi.status()) == WL_DISCONNECTED && counter * 2 < timeout) {
                delay(500);
                digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
                counter++;
            }
            if (connect_status != WL_CONNECTED) {
                DEBUG_OUT(F("Failed to connect!"));
            }
            WiFi.persistent(false);
        }
        digitalWrite(STATUS_LED, 1);
    }

    void init_ap() {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(ip_local, ip_gateway, ip_subnet);
        WiFi.softAP(config::global_config.ap_ssid, config::global_config.ap_password);
    }

    void init_server(ESP8266WebServer &server) {
        server.on("/", HTTP_GET, [&server]() {
            server.sendHeader("Location", "/config");
            server.send(301, "text/plain", "");
        });
        server.on("/stylesheet.css", HTTP_GET, [&server]() {
            server.send_P(200, "text/css", PSTR(
                "body {font-family: \"Helvetica Neue\", Helvetica, Arial, sans-serif;font-size: 16px;}details {display: inline-block;padding: 7px;border-radius: 5px;border: 1px solid #aaa;}form, p {margin: 5px;}input {margin-top: 3px;margin-bottom: 3px;}"
            ));
        });
        // For debug
        server.on("/status", HTTP_GET, [&server]() {
            char buf[20];
            snprintf_P(buf, sizeof(buf), "Status: %d", connect_status);
            server.send(200, "text/plain", buf);
        });
        // Main configuration UI
        server.on("/config", HTTP_GET, [&server]() {
            const char *status_str;
            switch (connect_status) {
            case WL_NO_SSID_AVAIL:
                status_str = PSTR("Connect Failed (Invalid SSID)");
                break;
            case WL_CONNECTED:
                status_str = PSTR("Connected");
                break;
            case WL_CONNECT_FAILED:
                status_str = PSTR("Connect Failed");
                break;
            case WL_WRONG_PASSWORD:
                status_str = PSTR("Connect Failed (Wrong Password)");
                break;
            case WL_DISCONNECTED:
                status_str = PSTR("Not Connected");
                break;
            case -2:
                status_str = PSTR("Not Connected (Manually Skipped)");
                break;
            default:
                status_str = PSTR("Unknown");
                break;
            }
            char buf[2048];
            snprintf_P(buf, sizeof(buf), PSTR(
                "<!DOCTYPE html><html><head> <title>Configuration</title> <link rel=\"stylesheet\" type=\"text/css\" href=\"stylesheet.css\"></head><body> <h2>Setup</h2> <details> <summary>Click to show current password</summary> %s </details> <form method=\"post\" action=\"/db-credentials\" enctype=\"application/x-www-form-urlencoded\"> <label>Email:<br><input type=\"text\" name=\"email\" value=\"%s\" required></label><br><label>Password:<br><input type=\"password\" name=\"password\" required></label><br><label>Display Data Location:<br><input type=\"text\" name=\"location\" required pattern=\"[a-zA-Z0-9-]{1,31}\" value=\"%s\"></label><br><input type=\"submit\" value=\"Update\"> </form> <h2>Connect to Wi-Fi</h2> <p>Current status: %s</p><form method=\"post\" action=\"/wifi-connect\" enctype=\"application/x-www-form-urlencoded\"> <label>Network Name (SSID):<br><input type=\"text\" name=\"ssid\" required></label><br><label>Password:<br><input type=\"password\" name=\"password\" required></label><br><input type=\"submit\" value=\"Connect\"> </form><br><h2>Configure Hosted Access Point Settings</h2> <details> <summary>Click to show current password</summary> %s </details> <form method=\"post\" action=\"/ap-setup\" enctype=\"application/x-www-form-urlencoded\"> <label>Network Name (SSID):<br><input type=\"text\" name=\"ssid\" value=\"%s\" required></label><br><label>Password:<br><input type=\"password\" name=\"password\" required></label><br><input type=\"submit\" value=\"Update\"> </form></body></html>"
            ), config::global_config.db_auth_password, config::global_config.db_auth_email, config::global_config.db_data_location, status_str, config::global_config.ap_password, config::global_config.ap_ssid);

            server.send(200, "text/html", buf);
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
        server.on("/db-credentials", HTTP_POST, [&server]() {
            if (!server.hasArg("email") && server.hasArg("password") && server.hasArg("location")) {
                server.send(400, "text/plain", "Bad Request");
                return;
            }
            strncpy(config::global_config.db_auth_email, server.arg("email").c_str(), 64);
            strncpy(config::global_config.db_auth_password, server.arg("password").c_str(), 32);
            strncpy(config::global_config.db_data_location, server.arg("location").c_str(), 32);
            config::save_config();
            server.send_P(200, "text/html", PSTR(
                "<!DOCTYPE html><html><head><title>Success!</title></head><body><p>Database setup configured successfully. You may now reset the device.</p></body></html>"
            ));
        });
        server.on("/ap-setup", HTTP_POST, [&server]() {
            if (!server.hasArg("ssid") && server.hasArg("password")) {
                server.send(400, "text/plain", "Bad Request");
                return;
            }
            strncpy(config::global_config.ap_ssid, server.arg("ssid").c_str(), 32);
            strncpy(config::global_config.ap_password, server.arg("password").c_str(), 32);
            config::save_config();
            server.send_P(200, "text/html", PSTR(
                "<!DOCTYPE html><html><head><title>Success!</title></head><body><p>Access point settings updated successfully. New settings will take effect on next reset.</p></body></html>"
            ));
        });
    }
}

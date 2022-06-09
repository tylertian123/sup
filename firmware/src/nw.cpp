#include "nw.h"
#include "common.h"
#include "config.h"
#include "page_content.h"

#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <stdio.h>
#include <string.h>


void quoted(char *buf, const char *str, char quote = '\"') {
    buf[0] = quote;
    strcpy(buf + 1, str);
    int len = strlen(str);
    buf[len] = quote;
    buf[len + 1] = '\0';
}


namespace nw {

    const char *hostname = "sup";
    const unsigned short timeout = 60;

    const IPAddress ip_local(192, 168, 0, 1);
    const IPAddress ip_gateway(192, 168, 0, 1);
    const IPAddress ip_subnet(255, 255, 255, 0);

    int connect_status = WL_DISCONNECTED;
    String input_ssid, input_password;

    void enterprise_connect(const char *ssid, const char *username, const char *identity, const char *password) {
        wifi_set_opmode(STATION_MODE);

        station_config conf;
        memset(&conf, 0, sizeof(station_config));
        strncpy(reinterpret_cast<char *>(conf.ssid), ssid, 32);
        strncpy(reinterpret_cast<char *>(conf.password), password, 64);
        wifi_station_set_config_current(&conf);

        wifi_station_set_wpa2_enterprise_auth(true);
        wifi_station_clear_cert_key();
        wifi_station_clear_enterprise_ca_cert();
        wifi_station_clear_enterprise_identity();
        wifi_station_clear_enterprise_username();
        wifi_station_clear_enterprise_password();
        wifi_station_clear_enterprise_new_password();

        wifi_station_set_enterprise_identity(const_cast<u8*>(reinterpret_cast<const u8*>(identity)), strlen(identity));
        wifi_station_set_enterprise_username(const_cast<u8*>(reinterpret_cast<const u8*>(username)), strlen(username));
        wifi_station_set_enterprise_password(const_cast<u8*>(reinterpret_cast<const u8*>(password)), strlen(password));

        wifi_station_connect();
    }

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
            DEBUG_OUT_LN(F("Connected to saved network"));
            digitalWrite(STATUS_LED, 1);
            return;
        }
        DEBUG_OUT_LN(F("Can't connect to saved network"));


        // In case of failure, run the access point + config server
        while (connect_status != WL_CONNECTED) {
            digitalWrite(STATUS_LED, 0);

            // Enable access point
            init_ap();
            if (!MDNS.begin(hostname)) {
                DEBUG_OUT_LN(F("Can't start mDNS"));
            }
            {
                // Run the server until we have the SSID and password
                ESP8266WebServer server(80);
                init_server(server);
                server.begin();
                DEBUG_OUT_LN(F("Server is online"));
                while (connect_status != -1) {
                    MDNS.update();
                    server.handleClient();
                }
            }
            // This delay seems to be necessary, otherwise the last page will time out
            delay(2000);
            WiFi.softAPdisconnect(true);

            DEBUG_OUT_LN(F("Connecting to SSID/Password:"));
            DEBUG_OUT_LN(input_ssid);
            DEBUG_OUT_LN(input_password);

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
                DEBUG_OUT_LN(F("Failed to connect!"));
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
            server.send_P(200, "text/css", PSTR(PAGE_CONTENT_STYLESHEET_CSS));
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
            
            snprintf_P(buf, sizeof(buf), PSTR(PAGE_CONTENT_CONFIG_HTML),
                config::global_config.db_auth_password,
                config::global_config.db_auth_email,
                config::global_config.db_data_location,
                status_str,
                config::global_config.ap_password,
                config::global_config.ap_ssid);

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

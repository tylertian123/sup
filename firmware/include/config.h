#pragma once

#include <stdint.h>

namespace config {

    constexpr uint8_t VERSION = 0x01;

    struct Config {
        uint8_t version = VERSION;
        
        char ap_ssid[32];
        char ap_password[32];

        char db_auth_email[64];
        char db_auth_password[32];
        char db_data_location[32];

        bool ent_enabled;
        char ent_ssid[32];
        char ent_username[32];
        char ent_password[64];
    };

    extern Config global_config;

    void init();
    void init_config_object(Config &conf);
    bool load_config(Config &conf = global_config, int addr = 0);
    void save_config(Config &conf = global_config, int addr = 0);
}

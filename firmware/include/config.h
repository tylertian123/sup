#pragma once

#include <stdint.h>

namespace config {
    struct Config {
        char db_auth_email[64];
        char db_auth_password[32];
    };

    extern Config global_config;

    void load_config(Config &conf = global_config);
    void save_config(Config &conf = global_config);
}

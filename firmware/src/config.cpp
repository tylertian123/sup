#include "config.h"
#include "secrets.h"

#include <EEPROM.h>
#include <string.h>

namespace config {

    Config global_config;

    void init() {
        EEPROM.begin(512);
    }

    void init_config_object(Config &conf) {
        conf = {};
        strncpy_P(conf.ap_ssid, PSTR(DEFAULT_SSID), sizeof(conf.ap_ssid));
        strncpy_P(conf.ap_password, PSTR(DEFAULT_PASSWORD), sizeof(conf.ap_password));
    }
    
    bool load_config(Config &conf, int addr) {
        EEPROM.get(addr, conf);
        // Check if version is correct
        if (conf.version == config::VERSION) {
            return true;
        }
        init_config_object(conf);
        return false;
    }

    void save_config(Config &conf, int addr) {
        EEPROM.put(addr, conf);
    }
}

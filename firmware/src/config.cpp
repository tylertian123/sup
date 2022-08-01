#include "config.h"
#include "secrets.h"
#include "common.h"

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
        conf.wifi_ccode[0] = 'C';
        conf.wifi_ccode[1] = 'A';
        conf.wifi_nchan = 11;
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
        EEPROM.commit();
        DEBUG_OUT_LN(F("Config saved"));
    }
}

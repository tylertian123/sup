#include "config.h"

#include <EEPROM.h>

namespace config {

    bool eeprom_init = false;
    Config global_config;
    
    void load_config(Config &conf) {
        if (!eeprom_init) {
            EEPROM.begin(512);
            eeprom_init = true;
        }
        EEPROM.get(0, conf);
    }

    void save_config(Config &conf) {
        if (!eeprom_init) {
            EEPROM.begin(512);
            eeprom_init = true;
        }
        EEPROM.put(0, conf);
    }
}

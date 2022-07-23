#pragma once

#include <Firebase_ESP_Client.h>

#include "wiring.h"

namespace fb {

    extern bool disp_data_updated;
    extern uint8_t disp_data[DISP_HEIGHT * 8][DISP_WIDTH];

    bool init();
    bool start_stream();
    bool check_ota();
    bool ota_update();
}

#pragma once

#include <stdint.h>

namespace max7219 {
    
    constexpr uint8_t DISP_MOD_WIDTH = 4;
    constexpr uint8_t DISP_MOD_HEIGHT = 2;
    constexpr uint16_t DISP_MOD_COUNT = DISP_MOD_WIDTH * DISP_MOD_HEIGHT;
    constexpr uint16_t DISP_PX_WIDTH = DISP_MOD_WIDTH * 8;
    constexpr uint16_t DISP_PX_HEIGHT = DISP_MOD_HEIGHT * 8;

    extern uint16_t cmd_buf[DISP_MOD_COUNT];

    constexpr uint8_t CMD_NOP = 0x00;
    constexpr uint8_t CMD_D0 = 0x01;
    constexpr uint8_t CMD_D1 = 0x01;
    constexpr uint8_t CMD_D2 = 0x01;
    constexpr uint8_t CMD_D3 = 0x01;
    constexpr uint8_t CMD_D4 = 0x01;
    constexpr uint8_t CMD_D5 = 0x01;
    constexpr uint8_t CMD_D6 = 0x01;
    constexpr uint8_t CMD_D7 = 0x01;
    constexpr uint8_t CMD_DECODE_MODE = 0x09;
    constexpr uint8_t CMD_INTENSITY = 0x0A;
    constexpr uint8_t CMD_SCAN_LIMIT = 0x0B;
    constexpr uint8_t CMD_SHUTDOWN = 0x0C;
    constexpr uint8_t CMD_DISP_TEST = 0x0F;

    void init_io();
    void init();
    void write_cmd(uint16_t *buf, uint8_t addr, uint8_t data);
    void fill_cmd(uint8_t addr, uint8_t data);
    void update();
}

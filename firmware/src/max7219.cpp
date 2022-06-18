#include "max7219.h"

#include <SPI.h>

namespace max7219 {
    
    alignas(uint32_t) uint16_t cmd_buf[DISP_MOD_COUNT];

    void init_io() {
        // 10MHz, MSB first, output on rising edge
        SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE1));
        SPI.begin();
        SPI.setHwCs(true);
        // Reuse MISO as OE for the logic level converter
        pinMode(MISO, OUTPUT);
        digitalWrite(MISO, 1);
    }

    void init() {
        init_io();
        fill_cmd(CMD_DISP_TEST, 0);
        update();
        fill_cmd(CMD_SCAN_LIMIT, 7);
        update();
        fill_cmd(CMD_SHUTDOWN, 1);
        update();
    }

    void write_cmd(uint16_t *buf, uint8_t addr, uint8_t data) {
        // Low-endian system
        *buf = (data << 8) | addr;
    }

    void fill_cmd(uint8_t addr, uint8_t data) {
        for (uint16_t i = 0; i < DISP_MOD_COUNT; i ++) {
            write_cmd(cmd_buf + i, addr, data);
        }
    }

    void update() {
        SPI.writeBytes(reinterpret_cast<uint8_t *>(cmd_buf), sizeof(cmd_buf));
    }
}

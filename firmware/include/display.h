#pragma once

#include <stdint.h>
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <pgmspace.h>
#include <algorithm>

#include "common.h"

#include "wiring.h"

namespace display {

    template <uint8_t Size>
    class MAX7219 {
    private:
        const uint8_t CS;

    public:
        enum Cmd : uint8_t {
            NOP = 0x00,
            DIG0 = 0x01,
            DIG1 = 0x02,
            DIG2 = 0x03,
            DIG3 = 0x04,
            DIG4 = 0x05,
            DIG5 = 0x06,
            DIG6 = 0x07,
            DIG7 = 0x08,
            DECODE_MODE = 0x09,
            INTENSITY = 0x0A,
            SCAN_LIMIT = 0x0B,
            SHUTDOWN = 0x0C,
            DISP_TEST = 0x0F,
        };

        static constexpr uint8_t width = Size * 8;
        alignas(uint32_t) uint16_t cmd_buf[Size];

        MAX7219(uint8_t CS) : CS(CS) {}

        // Initialize pin modes and SPI
        void init_io() {
            // 10MHz, MSB first, output on rising edge
            SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE1));
            SPI.begin();
            // Control CS line manually
            pinMode(CS, OUTPUT);
            digitalWrite(CS, 0);
        }

        // Configure all displays
        void init(uint8_t intensity = 0) {
            // Turn off display test mode
            fill_cmd(DISP_TEST, 0);
            update();
            // Turn on all 7 lines
            fill_cmd(SCAN_LIMIT, 7);
            update();
            // No decode
            fill_cmd(DECODE_MODE, 0);
            update();
            // Min intensity
            fill_cmd(INTENSITY, intensity);
            update();
            // Turn screen on
            fill_cmd(SHUTDOWN, 1);
            update();
        }

        // Write a command for a specific module into the buffer. Does NOT update the display.
        void write_cmd(uint8_t disp, uint8_t addr, uint8_t data) {
            // Flip for low-endian system
            cmd_buf[disp] = (data << 8) | addr;
        }
        
        // Write the same command for all modules in the buffer. Does NOT update the display.
        void fill_cmd(uint8_t addr, uint8_t data) {
            for (uint8_t i = 0; i < Size; i ++) {
                write_cmd(i, addr, data);
            }
        }

        // Clear the command buffer (set to all NOPs). Does NOT update the display.
        void clear_cmd() {
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        // Clear the display.
        void clear() {
            for (uint8_t i = 0; i < 8; i ++) {
                fill_cmd(DIG0 + i, 0);
                update();
            }
        }

        // Send the contents of the command buffer to the displays.
        void update() {
            digitalWrite(CS, 0);
            SPI.writeBytes(reinterpret_cast<uint8_t *>(cmd_buf), sizeof(cmd_buf));
            digitalWrite(CS, 1);
        }
    };

    template<uint8_t Width, uint8_t Height, uint8_t... Pins>
    class MAX7219Group {
    public:
        MAX7219<Width> rows[Height];
        uint8_t disp_buf[Height * 8][Width] = {0};

        static constexpr uint16_t mod_width = Width, mod_height = Height;
        static constexpr uint16_t width = Width * 8, height = Height * 8;

        MAX7219Group() : rows{Pins...}, disp_buf{} {}

        // Initialize I/O, turn on and clear all displays.
        void init(uint8_t intensity = 0) {
            for (uint8_t i = 0; i < Height; i ++) {
                rows[i].init_io();
                // Extra redundancy in init sequence
                rows[i].init(intensity);
                rows[i].init(intensity);
                rows[i].init(intensity);
                rows[i].clear();
            }
        }

        // Clear the display buffer. Does NOT update the display.
        void clear_buf() {
            memset(disp_buf, 0, sizeof(disp_buf));
        }

        // Update the display to what the display buffer contains.
        void update() {
            for (int i = 0; i < Height; i ++) {
                MAX7219<Width> &disp = rows[i];
                for (int j = 0; j < 8; j ++) {
                    uint8_t *row = disp_buf[i * 8 + j];
                    for (int k = 0; k < Width; k ++) {
                        disp.write_cmd(k, MAX7219<Width>::DIG0 + j, row[k]);
                    }
                    disp.update();
                }
            }
        }

        // Write a command to all the display modules.
        void write_all(uint8_t addr, uint8_t data) {
            for (int i = 0; i < Height; i ++) {
                rows[i].fill_cmd(addr, data);
                rows[i].update();
            }
        }

        void set_pixel(uint8_t x, uint8_t y) {
            disp_buf[y][x / 8] |= 0x80 >> (x % 8);
        }
        void clear_pixel(uint8_t x, uint8_t y) {
            disp_buf[y][x / 8] &= 0xFF - (0x80 >> (x % 8));
        }
        void set_pixel(uint8_t x, uint8_t y, bool state) {
            if (state) {
                set_pixel(x, y);
            }
            else {
                clear_pixel(x, y);
            }
        }
    };

    using Display = MAX7219Group<DISP_WIDTH, DISP_HEIGHT, DISP1_CS, DISP2_CS>;
}

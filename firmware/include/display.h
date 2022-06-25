#pragma once

#include <stdint.h>
#include <string.h>
#include <Arduino.h>

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

        void init_io() {
            // 10MHz, MSB first, output on rising edge
            SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE1));
            SPI.begin();
            // Control CS line manually
            pinMode(CS, OUTPUT);
            digitalWrite(CS, 0);
        }

        void init() {
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
            fill_cmd(INTENSITY, 0);
            update();
            // Turn screen on
            fill_cmd(SHUTDOWN, 1);
            update();
        }

        void write_cmd(uint8_t disp, uint8_t addr, uint8_t data) {
            // Flip for low-endian system
            cmd_buf[disp] = (data << 8) | addr;
        }
        
        void fill_cmd(uint8_t addr, uint8_t data) {
            for (uint8_t i = 0; i < Size; i ++) {
                write_cmd(i, addr, data);
            }
        }

        void clear_cmd() {
            memset(cmd_buf, 0, sizeof(cmd_buf));
        }

        void clear() {
            for (uint8_t i = 0; i < 8; i ++) {
                fill_cmd(DIG0 + i, 0);
                update();
            }
        }

        void update() {
            digitalWrite(CS, 0);
            SPI.writeBytes(reinterpret_cast<uint8_t *>(cmd_buf), sizeof(cmd_buf));
            digitalWrite(CS, 1);
        }
    };

    template<uint8_t Width, uint8_t Height, uint8_t... Pins>
    class Display {
    public:
        MAX7219<Width> rows[Height];
        uint8_t disp_buf[Height * 8][Width];

        static constexpr uint16_t mod_width = Width, mod_height = Height;
        static constexpr uint16_t width = Width * 8, height = Height * 8;

        Display() : rows{Pins...}, disp_buf{} {}

        void init() {
            for (uint8_t i = 0; i < Height; i ++) {
                rows[i].init_io();
                // Extra redundancy in init sequence
                rows[i].init();
                rows[i].init();
                rows[i].init();
                rows[i].clear();
            }
        }

        void clear_buf() {
            memset(disp_buf, 0, sizeof(disp_buf));
        }

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
    };
}

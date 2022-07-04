#include "graphics.h"
#include "display.h"

#include <pgmspace.h>
#include <stdint.h>


namespace graphics {
    constexpr uint16_t GLYPH_BUF_SIZE = 64;

    void Glyph::draw_P(display::Display &disp, int16_t x, int16_t y) {
        Glyph glyph;
        uint8_t ramData[GLYPH_BUF_SIZE];
        // Copy glyph into and location of data into RAM
        memcpy_P(&glyph, this, sizeof(Glyph));
        // Copy pixel data into ram
        memcpy_P(ramData, glyph.data, glyph.width_bytes * glyph.height);
        // Point the data location to the location in ram
        glyph.data = ramData;
        
        glyph.draw(disp, x, y);
    }

    void Glyph::draw(display::Display &disp, int16_t x, int16_t y) {
        for (uint8_t row = std::max<int16_t>(y, 0); row < std::min<int16_t>(y + height, disp.height); row ++) {
            const uint8_t *row_data = data + (row - y) * width_bytes;
            for (uint8_t col = std::max<int16_t>(x, 0); col < std::min<int16_t>(x + width, disp.width); col ++) {
                disp.set_pixel(col, row, row_data[(col - x) / 8] & (0x80 >> (col - x) % 8));
            }
        }
    }

    void draw_str(display::Display &disp, const char *str, int16_t x, int16_t y) {
        if (y > disp.height) {
            return;
        }
        for (; *str; str ++) {
            Glyph glyph;
            // Map the current character to a glyph, and copy it from PROGMEM to RAM
            memcpy_P(&glyph, &map_char(*str), sizeof(graphics::Glyph));
            if (x + glyph.width < 0 || y + glyph.height < 0) {
                x += glyph.width + 1;
                continue;
            }
            // Copy its data into RAM
            uint8_t data[GLYPH_BUF_SIZE];
            memcpy_P(data, glyph.data, glyph.width_bytes * glyph.height);
            glyph.data = data;

            glyph.draw(disp, x, y);
            x += glyph.width + 1;
            if (x >= disp.width) {
                return;
            }
        }
    }
}

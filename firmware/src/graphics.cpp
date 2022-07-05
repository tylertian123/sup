#include "graphics.h"
#include "display.h"

#include <pgmspace.h>
#include <stdint.h>
#include <algorithm>


namespace graphics {
    constexpr uint16_t GLYPH_BUF_SIZE = 64;

    void Region::constrain_to_screen() {
        min_x = std::max<int16_t>(0, min_x);
        min_y = std::max<int16_t>(0, min_y);
        max_x = std::min<int16_t>(display::Display::width, max_x);
        max_y = std::min<int16_t>(display::Display::height, max_y);
    }

    void Glyph::draw_P(display::Display &disp, int16_t x, int16_t y, Region r) const {
        Glyph glyph;
        uint8_t ramData[GLYPH_BUF_SIZE];
        // Copy glyph into and location of data into RAM
        memcpy_P(&glyph, this, sizeof(Glyph));
        // Copy pixel data into ram
        memcpy_P(ramData, glyph.data, glyph.width_bytes * glyph.height);
        // Point the data location to the location in ram
        glyph.data = ramData;
        
        glyph.draw(disp, x, y, r);
    }

    void Glyph::draw(display::Display &disp, int16_t x, int16_t y, Region r) const {
        r.constrain_to_screen();
        for (uint8_t row = std::max<int16_t>(y, r.min_y); row < std::min<int16_t>(y + height, r.max_y); row ++) {
            const uint8_t *row_data = data + (row - y) * width_bytes;
            for (uint8_t col = std::max<int16_t>(x, r.min_x); col < std::min<int16_t>(x + width, r.max_x); col ++) {
                disp.set_pixel(col, row, row_data[(col - x) / 8] & (0x80 >> (col - x) % 8));
            }
        }
    }

    void ScrollingText::set_str(const char *str) {
        this->str = str;
        // Recompute text width
        text_width = str_width(str);
        // Reset scrolling parameters
        last_update = millis();
        scroll_offset = 0;
    }
    
    bool ScrollingText::update() {
        unsigned long t = millis();
        // Scroll by 1 pixel if scroll delay time reached
        if (t - last_update > SCROLL_DELAY) {
            last_update += SCROLL_DELAY;
            scroll_offset ++;
            // Wrap around
            if (scroll_offset >= text_width + EMPTY_SPACE) {
                scroll_offset = 0;
            }
            return true;
        }
        return false;
    }

    void ScrollingText::draw(display::Display &disp) {
        if (!update()) {
            return;
        }
        clear(disp, region);
        draw_str(disp, str, x - scroll_offset, y, region);
        // Draw second part if if it's on screen
        if (x - scroll_offset + text_width + EMPTY_SPACE < region.max_x) {
            draw_str(disp, str, x - scroll_offset + text_width + EMPTY_SPACE, y, region);
        }
    }

    const PROGMEM uint8_t SPINNER_DATA[] = {
        _PAD(0b01110),
        _PAD(0b10001),
        _PAD(0b10001),
        _PAD(0b10001),
        _PAD(0b01110),
    };
    const PROGMEM Glyph SPINNER(SPINNER_DATA, 5, 5);
    const uint8_t SPINNER_ANIM_X[] = {2, 3, 4, 4, 4, 3, 2, 1, 0, 0, 0, 1};
    const uint8_t SPINNER_ANIM_Y[] = {0, 0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0};
    constexpr uint8_t SPINNER_ANIM_LEN = sizeof(SPINNER_ANIM_X);

    bool Spinner::update() {
        unsigned long t = millis();
        if (t - last_update > SPIN_DELAY) {
            last_update += SPIN_DELAY;
            state ++;
            if (state >= SPINNER_ANIM_LEN) {
                state = 0;
            }
            return true;
        }
        return false;
    }

    void Spinner::draw(display::Display &disp) {
        if (!update()) {
            return;
        }
        clear(disp, {x, x + 5, y, y + 5});
        SPINNER.draw_P(disp, x, y);
        disp.clear_pixel(SPINNER_ANIM_X[state], SPINNER_ANIM_Y[state]);
    }

    void clear(display::Display &disp, Region region) {
        region.constrain_to_screen();
        // Some clever bitwise trickery would make this much faster
        // but CPU time is quite cheap right now
        for (uint8_t y = region.min_y; y < region.max_y; y ++) {
            for (uint8_t x = region.min_x; x < region.max_x; x ++) {
                disp.clear_pixel(x, y);
            }
        }
    }

    void draw_str(display::Display &disp, const char *str, int16_t x, int16_t y, Region r) {
        if (y >= r.max_y) {
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

            glyph.draw(disp, x, y, r);
            x += glyph.width + 1;
            if (x >= r.max_x) {
                return;
            }
        }
    }

    uint16_t str_width(const char *str) {
        // To make an empty string have width 0
        uint16_t w = str ? 1 : 0;
        for (; *str; str++) {
            Glyph glyph;
            memcpy_P(&glyph, &map_char(*str), sizeof(graphics::Glyph));
            w += glyph.width + 1;
        }
        // -1 for the extra spacing of the last character
        return w - 1;
    }
}
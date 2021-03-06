#include "graphics.h"
#include "display.h"

#include <pgmspace.h>
#include <stdint.h>
#include <algorithm>


namespace graphics {
    constexpr uint16_t GLYPH_BUF_SIZE = 64;

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
            uint8_t data[GLYPH_BUF_SIZE];
            // Make an instance in RAM
            Glyph glyph(map_char(*str), data);
            if (x + glyph.width < 0 || y + glyph.height < 0) {
                x += glyph.width + 1;
                continue;
            }

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

    void Region::constrain_to_screen() {
        min_x = std::max<int16_t>(0, min_x);
        min_y = std::max<int16_t>(0, min_y);
        max_x = std::min<int16_t>(display::Display::width, max_x);
        max_y = std::min<int16_t>(display::Display::height, max_y);
    }

    Glyph::Glyph(const Glyph &progmem_glyph, void *data_ptr) {
        memcpy_P(this, &progmem_glyph, sizeof(Glyph));
        if (data_ptr) {
            memcpy_P(data_ptr, data, width_bytes * height);
            data = static_cast<uint8_t *>(data_ptr);
        }
    }

    void Glyph::draw_P(display::Display &disp, int16_t x, int16_t y, Region r) const {
        uint8_t ram_data[GLYPH_BUF_SIZE];
        Glyph g(*this, ram_data);
        g.draw(disp, x, y, r);
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

    void DisplayElement::set_position(int16_t x, int16_t y, uint16_t width, uint16_t height) {
        region = {
            .min_x = x,
            .max_x = static_cast<int16_t>(x + width),
            .min_y = y,
            .max_y = static_cast<int16_t>(y + height)
        };
        updated = true;
    }

    bool DisplayElement::draw(display::Display &disp, bool force) {
        // Check that the element needs to be redrawn
        if (!force && !updated) {
            return false;
        }
        updated = true;
        return _draw(disp);
    }

    void ProgressBar::set_max_progress(uint32_t max) {
        this->max = max;
        updated = true;
    }

    void ProgressBar::set_progress(uint32_t p) {
        progress = (region.max_x - region.min_x) * p / max;
        updated = true;
    }

    bool ProgressBar::_draw(display::Display &disp) {
        clear(disp, region);
        // Box
        for (uint16_t x = region.min_x; x < region.max_x; x ++) {
            disp.set_pixel(x, region.min_y);
            disp.set_pixel(x, region.max_y - 1);
        }
        for (uint16_t y = region.min_y; y < region.max_y; y ++) {
            disp.set_pixel(region.min_x, y);
            disp.set_pixel(region.max_x - 1, y);
        }
        // Bar
        for (uint16_t x = 1; x < progress; x ++) {
            for (int16_t y = region.min_y + 1; y < region.max_y - 1; y ++) {
                disp.set_pixel(region.min_x + x, y);
            }
        }
        return true;
    }

    void ScrollingText::set_str(const char *str) {
        strncpy(text, str, sizeof(text));
        // Recompute text width
        text_width = str_width(str);
        scroll = region.min_x + text_width >= region.max_x;
        // Reset scrolling parameters
        scroll_offset = -1;
        updated = true;
    }

    void ScrollingText::set_str(const __FlashStringHelper *str) {
        strncpy_P(text, reinterpret_cast<const char *>(str), sizeof(text));
        text_width = str_width(text);
        scroll = region.min_x + text_width >= region.max_x;
        scroll_offset = 0;
        scroll_start = millis() + (region.max_x - region.min_x) * 15;
        updated = true;
    }
    
    bool ScrollingText::_update(unsigned long t) {
        if (scroll && t > scroll_start) {
            scroll_offset ++;
            // Wrap around
            if (scroll_offset >= text_width + EMPTY_SPACE) {
                scroll_offset = 0;
            }
            return true;
        }
        return false;
    }

    bool ScrollingText::_draw(display::Display &disp) {
        clear(disp, region);
        draw_str(disp, text, region.min_x - scroll_offset, region.min_y, region);
        // Draw second part if if it's on screen
        if (scroll && region.min_x - scroll_offset + text_width + EMPTY_SPACE < region.max_x) {
            draw_str(disp, text, region.min_x - scroll_offset + text_width + EMPTY_SPACE, region.min_y, region);
        }
        return true;
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

    bool Spinner::_update(unsigned long t) {
        state ++;
        if (state >= SPINNER_ANIM_LEN) {
            state = 0;
        }
        return true;
    }

    bool Spinner::_draw(display::Display &disp) {
        clear(disp, region);
        SPINNER.draw_P(disp, region.min_x, region.min_y);
        disp.clear_pixel(region.min_x + SPINNER_ANIM_X[state], region.min_y + SPINNER_ANIM_Y[state]);
        return true;
    }
}

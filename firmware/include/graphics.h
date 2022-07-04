#pragma once

#include <stdint.h>
#include "display.h"

namespace graphics {
    class Glyph {
    public:
        const uint8_t *data;
        uint8_t width;
        uint8_t height;
        uint8_t width_bytes;
        
        constexpr Glyph(const uint8_t *data, uint8_t width, uint8_t height)
            // Width is divided by 8 and rounded up to get byte width
            : data(data), width(width), height(height), width_bytes((width - 1) / 8 + 1) {}
        constexpr Glyph() : data(nullptr), width(0), height(0), width_bytes(0) {}
        
        void draw(display::Display &disp, int16_t x, int16_t y);
        void draw_P(display::Display &disp, int16_t x, int16_t y);
    };

    class ScrollingText {
    
    public:
        void draw(display::Display &disp);
    };

    const Glyph& map_char(char c);

    void draw_str(display::Display &disp, const char *str, int16_t x, int16_t y);
}

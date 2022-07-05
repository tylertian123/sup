#pragma once

#include <stdint.h>
#include "display.h"

// Right pads binary literal with 0s to make it 1 byte
// sizeof(#x) - 3 subtracts 3 for 0b prefix + null terminator
#define _PAD(x) (x << (8 - (sizeof(#x) - 3)))

namespace graphics {
    const struct Region {
        // Inclusive
        int16_t min_x = 0;
        // Exclusive
        int16_t max_x = display::Display::width;
        // Inclusive
        int16_t min_y = 0;
        // Exclusive
        int16_t max_y = display::Display::height;

        void constrain_to_screen();
    } FULL_SCREEN;

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
        
        void draw(display::Display &disp, int16_t x, int16_t y, Region region = FULL_SCREEN) const;
        void draw_P(display::Display &disp, int16_t x, int16_t y, Region region = FULL_SCREEN) const;
    };

    class ScrollingText {
    private:
        static constexpr unsigned int SCROLL_DELAY = 75;
        static constexpr uint16_t EMPTY_SPACE = 5;

        int16_t x, y;
        Region region;
        
        const char *str = nullptr;
        uint16_t text_width = 0;

        unsigned long last_update = 0;
        uint16_t scroll_offset = 0;

    public:
        ScrollingText(int16_t x, int16_t y, uint16_t width, uint16_t height)
            : x(x), y(y), region{x, x + width, y, y + height} {}
        void set_str(const char *str);
        bool update();
        void draw(display::Display &disp);
    };

    class Spinner {
    private:
        static constexpr unsigned int SPIN_DELAY = 75;
        int16_t x, y;

        unsigned long last_update = 0;
        uint8_t state = 0;
    
    public:
        Spinner(int16_t x, int16_t y) : x(x), y(y) {};
        bool update();
        void draw(display::Display &disp);
    };

    const Glyph& map_char(char c);

    void clear(display::Display &disp, Region region = FULL_SCREEN);
    void draw_str(display::Display &disp, const char *str, int16_t x, int16_t y, Region region = FULL_SCREEN);
    uint16_t str_width(const char *str);
}

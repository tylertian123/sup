#pragma once

#include <stdint.h>
#include "display.h"
#include <Arduino.h>

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
        // Constructor for an object in RAM from an object in PROGMEM
        // Copies the actual glyph data to data_ptr unless it's null, and sets the data pointer to point to it
        Glyph(const Glyph &progmem_glyph, void *data_ptr);
        
        void draw(display::Display &disp, int16_t x, int16_t y, Region region = FULL_SCREEN) const;
        void draw_P(display::Display &disp, int16_t x, int16_t y, Region region = FULL_SCREEN) const;
    };

    class ScrollingText {
    private:
        static constexpr unsigned int SCROLL_DELAY = 50;
        static constexpr uint16_t EMPTY_SPACE = 8;

        Region region;
        
        char text[64] = {0};
        uint16_t text_width = 0;
        bool scroll = false;

        unsigned long last_update = 0;
        int16_t scroll_offset = 0;

    public:
        ScrollingText(int16_t x, int16_t y, uint16_t width, uint16_t height)
            : region{x, static_cast<int16_t>(x + width), y, static_cast<int16_t>(y + height)} {}
        void set_str(const char *str);
        bool update(unsigned long t);
        bool draw(display::Display &disp, unsigned long t, bool force = false);
    };

    class Spinner {
    private:
        static constexpr unsigned int SPIN_DELAY = 75;
        int16_t x, y;

        unsigned long last_update = 0;
        uint8_t state = 0;
    
    public:
        Spinner(int16_t x, int16_t y) : x(x), y(y) {};
        bool update(unsigned long t);
        bool draw(display::Display &disp, unsigned long t, bool force = false);
    };

    class ProgressBar {
    private:
        uint32_t max = 0;
        uint8_t progress = 0;
        Region region;
        bool updated = true;
    
    public:
        ProgressBar(int16_t x, int16_t y, uint16_t width, uint16_t height)
            : region{x, static_cast<int16_t>(x + width), y, static_cast<int16_t>(y + height)} {}
        void set_max_progress(uint32_t max);
        void set_progress(uint32_t p);
        bool draw(display::Display &disp, bool force = false);
    };

    const Glyph& map_char(char c);

    void clear(display::Display &disp, Region region = FULL_SCREEN);
    void draw_str(display::Display &disp, const char *str, int16_t x, int16_t y, Region region = FULL_SCREEN);
    uint16_t str_width(const char *str);
}

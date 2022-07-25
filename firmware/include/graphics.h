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

    // Base display element class
    class DisplayElement {
    protected:
        Region region;
        bool updated = true;

        // Override this to draw the element onto the screen
        // Only called when an update is needed
        virtual bool _draw(display::Display &disp) = 0;

    public:
        DisplayElement(int16_t x, int16_t y, uint16_t width, uint16_t height)
            : region{x, static_cast<int16_t>(x + width), y, static_cast<int16_t>(y + height)} {}
        void set_position(int16_t x, int16_t y, uint16_t width, uint16_t height);
        virtual bool draw(display::Display &disp, bool force = false);
    };

    class ProgressBar : public DisplayElement {
    private:
        uint32_t max = 0;
        uint8_t progress = 0;

    protected:
        virtual bool _draw(display::Display &disp) override;
    
    public:
        using DisplayElement::DisplayElement;
        // Set how much progress is out of
        void set_max_progress(uint32_t max);
        // Set the current progress (out of the max progress)
        void set_progress(uint32_t p);
    };

    // Base animated display element class
    template <uint16_t FrameDelay>
    class AnimatedElement : public DisplayElement {
    protected:
        unsigned long last_update = 0;

        // Override this to implement state update logic for each frame
        virtual bool _update(unsigned long t) = 0;
    
    public:
        static constexpr uint16_t FRAME_DELAY = FrameDelay;
    
        using DisplayElement::DisplayElement;

        virtual bool update(unsigned long t) {
            // Update if required time has passed
            if (t - last_update > FRAME_DELAY) {
                last_update += FRAME_DELAY * ((t - last_update) / FRAME_DELAY);
                return _update(t);
            }
            // Otherwise return whether a static update has occurred
            return updated;
        }

        virtual bool draw(display::Display &disp, bool force = false) override {
            return draw(disp, millis(), force);
        }

        virtual bool draw(display::Display &disp, unsigned long t, bool force = false) {
            if (!force && !update(t)) {
                return false;
            }
            updated = true;
            return _draw(disp);
        }
    };

    class ScrollingText : public AnimatedElement<50> {
    private:
        static constexpr uint16_t EMPTY_SPACE = 8;
        
        char text[64] = {0};
        uint16_t text_width = 0;
        bool scroll = false;
        int16_t scroll_offset = 0;
        unsigned long scroll_start = 0;
    
    protected:
        virtual bool _update(unsigned long t) override;
        virtual bool _draw(display::Display &disp) override;

    public:
        using AnimatedElement::AnimatedElement;
        void set_str(const char *str);
        void set_str(const __FlashStringHelper *str);
    };

    class Spinner : public AnimatedElement<75> {
    private:
        uint8_t state = 0;

    protected:
        virtual bool _update(unsigned long t) override;
        virtual bool _draw(display::Display &disp) override;
    
    public:
        Spinner(int16_t x, int16_t y) : AnimatedElement(x, y, 5, 5) {};
    };

    const Glyph& map_char(char c);

    void clear(display::Display &disp, Region region = FULL_SCREEN);
    void draw_str(display::Display &disp, const char *str, int16_t x, int16_t y, Region region = FULL_SCREEN);
    uint16_t str_width(const char *str);
}

#pragma once

#include "display.h"
#include "wiring.h"
#include "graphics.h"

namespace ui {

    class Button {
    private:
        static constexpr unsigned int DEBOUNCE_DELAY = 10;
        static constexpr unsigned int HOLD_DELAY = 1000;
        const uint8_t pin;
        unsigned long last_change;
        // Whether the button has previously been held down
        // This is to prevent a "pressed" when the button is being released from being held down
        bool in_hold = false;

    public:
        // Whether the button is currently down
        bool down = false;
        // Set to true once when the button has been pressed and released; needs to be cleared
        bool pressed = false;
        // Set to true once after the button has been held down (but before release); needs to be cleared
        bool held = false;
        unsigned long held_duration = 0;

        Button(uint8_t pin) : pin(pin) {}
        void init();
        // Update the state of the button.
        void poll(unsigned long t = 0);
    };

    class LED {
    private:
        const uint8_t pin;
        unsigned long last_change;
        // Zero when not in blink mode
        uint16_t blink_duration = 0;
        // Note this counts the number of state changes, not actual blink cycles
        // 0xFFFF when blinking indefinitely
        uint16_t blink_counter = 0;
    
    public:
        static constexpr uint16_t INDEFINITE = 0xFFFF;

        LED(uint8_t pin) : pin(pin) {}
        void init(bool state = false);
        void set(bool state);
        void blink(uint16_t duration, uint16_t times = INDEFINITE);
        void poll(unsigned long t = 0);
    };

    extern display::Display disp;
    extern Button input1, input2;
    extern LED status_led, error_led;
    extern graphics::ProgressBar progress_bar;

    enum class LayoutType {
        LOADING_TEXT, LOADING_PROGRESS_BAR, ERROR_TEXT
    };

    void init();
    void set_text(const char *top, const char *bottom);
    void set_layout(LayoutType type);
    void poll();
}

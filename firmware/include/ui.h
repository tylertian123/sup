#pragma once

#include "display.h"
#include "wiring.h"

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
        void poll();
    };

    extern display::Display<DISP_WIDTH, DISP_HEIGHT, DISP1_CS, DISP2_CS> disp;
    extern Button input1, input2;

    void init();
    void poll(bool init_success);
}

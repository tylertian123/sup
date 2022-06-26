#include "ui.h"
#include "wiring.h"
#include "display.h"
#include "common.h"

#include <Arduino.h>

namespace ui {

    void Button::init() {
        pinMode(pin, INPUT_PULLUP);
        last_change = millis();
        down = !digitalRead(pin);
    }

    void Button::poll() {
        unsigned long t = millis();
        bool state = !digitalRead(pin);
        // Check for button being held
        // Require button being previously down and a certain amount of time before last state change
        if (down && state && t - last_change > HOLD_DELAY) {
            held = true;
            hold_release = true;
        }
        // Check for a state change with debounce
        if (state != down && t - last_change > DEBOUNCE_DELAY) {
            // Check for button being released
            if (!state) {
                // Only recognize it if the button is not being held down
                if (!hold_release) {
                    pressed = true;
                }
                hold_release = false;
            }
            down = state;
            last_change = t;
        }
    }

    display::Display<DISP_WIDTH, DISP_HEIGHT, DISP1_CS, DISP2_CS> disp;
    Button input1(INPUT1), input2(INPUT2);

    uint8_t disp_brightness = 0;

    void init() {
        // Init display first because SPI init changes the pin mode for some pins
        disp.init();
        input1.init();
        input2.init();
        pinMode(STATUS, OUTPUT);
    }

    void poll() {
        input1.poll();
        input2.poll();
        if (input1.pressed) {
            input1.pressed = false;
            if (disp_brightness < 31) {
                disp_brightness ++;
            }
            disp.write_all(display::MAX7219<DISP_WIDTH>::INTENSITY, disp_brightness);
        }
        if (input2.pressed) {
            input2.pressed = false;
            if (disp_brightness > 0) {
                disp_brightness --;
            }
            disp.write_all(display::MAX7219<DISP_WIDTH>::INTENSITY, disp_brightness);
        }
    }
}

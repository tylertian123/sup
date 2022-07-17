#include "ui.h"
#include "wiring.h"
#include "display.h"
#include "graphics.h"
#include "common.h"
#include "fb.h"

#include <Arduino.h>

namespace ui {

    void Button::init() {
        pinMode(pin, INPUT_PULLUP);
        last_change = millis();
        down = !digitalRead(pin);
    }

    void Button::poll(unsigned long t) {
        if (!t) {
            t = millis();
        }
        bool state = !digitalRead(pin);
        // Check for button being held
        // Require button being previously down and a certain amount of time before last state change
        if (down && state && !in_hold && t - last_change > HOLD_DELAY) {
            held = true;
            in_hold = true;
        }
        if (in_hold) {
            held_duration = t - last_change;
        }
        // Check for a state change with debounce
        if (state != down && t - last_change > DEBOUNCE_DELAY) {
            // Check for button being released
            if (!state) {
                // Only recognize it if the button is not being held down
                if (!in_hold) {
                    pressed = true;
                }
                in_hold = false;
                held_duration = 0;
            }
            down = state;
            last_change = t;
        }
    }

    void LED::init(bool state) {
        pinMode(pin, OUTPUT);
        set(state);
        last_change = millis();
    }

    void LED::set(bool state) {
        blink_duration = 0;
        blink_counter = 0;
        // Note state is inverted since writing a high turns off the LED
        digitalWrite(pin, !state);
    }

    void LED::blink(uint16_t duration, uint16_t times) {
        // Don't change if busy
        if (blink_duration) {
            return;
        }
        // Since the counter counts state changes, not blink cycles, this needs to be doubled
        blink_counter = times == INDEFINITE ? times : times * 2;
        blink_duration = duration;
    }

    void LED::poll(unsigned long t) {
        if (!t) {
            t = millis();
        }
        if (blink_duration) {
            // Check for state change
            if (t - last_change >= blink_duration) {
                // Add enough time for any missed changes
                last_change += blink_duration * ((t - last_change) / blink_duration);
                digitalWrite(pin, !digitalRead(pin));
                // Decrement counter, stop blinking if counter reaches 0
                if (blink_counter != INDEFINITE) {
                    blink_counter --;
                    if (!blink_counter) {
                        blink_duration = 0;
                    }
                }
            }
        }
        else {
            last_change = t;
        }
    }

    LayoutType layout = LayoutType::LOADING_TEXT;

    display::Display disp;
    Button input1(INPUT_BTN1), input2(INPUT_BTN2);
    LED status_led(STATUS_LED), error_led(ERROR_LED);

    graphics::ScrollingText top_text(6, 1, 26, 5);
    graphics::ScrollingText bottom_text(0, 8, 32, 5);
    graphics::Spinner spinner(0, 1);
    graphics::ProgressBar progress_bar(0, 9, 32, 6);
    bool redraw = false;

    bool has_data = false;
    uint8_t disp_brightness = 0;
    bool sleep = false;

    void init() {
        // Init display first because SPI init changes the pin mode for some pins
        disp.init();
        input1.init();
        input2.init();
        status_led.init();
        error_led.init();
        pinMode(STATUS_LED, OUTPUT);
        set_layout(LayoutType::LOADING_TEXT);
        set_text("Init", "Please wait");
    }

    void set_text(const char *top, const char *bottom) {
        if (top) {
            top_text.set_str(top);
        }
        if (bottom) {
            bottom_text.set_str(bottom);
        }
    }

    // If init is true, then the function should initialize the layout, i.e. redraw everything
    bool draw_layout_text(unsigned long t, bool init = false) {
        // This layout changes the position of the top text
        // Other layouts do not change it back; this layout is intended for use before a reboot for OTA updates
        if (init) {
            top_text.set_position(0, 1, 32, 5);
        }
        return top_text.draw(disp, t, init) | bottom_text.draw(disp, t, init);
    }

    bool draw_layout_loading_text(unsigned long t, bool init = false) {
        // Use bitwise OR to avoid short circuit so all 3 are always drawn
        return top_text.draw(disp, t, init) | bottom_text.draw(disp, t, init) | spinner.draw(disp, t, init);
    }
    
    bool draw_layout_loading_progress_bar(unsigned long t, bool init = false) {
        return top_text.draw(disp, t, init) | progress_bar.draw(disp, init) | spinner.draw(disp, t, init);
    }

    bool draw_layout_error_text(unsigned long t, bool init = false) {
        if (init) {
            // Draw the exclamation mark a single time
            uint8_t data[16];
            graphics::Glyph glyph(graphics::map_char('!'), data);
            glyph.draw(disp, (5 - glyph.width) / 2, (5 - glyph.height) / 2 + 1);
            redraw = true;
        }
        return top_text.draw(disp, t, init) | bottom_text.draw(disp, t, init);
    }

    bool (* const LAYOUT_HANDLERS[]) (unsigned long t, bool init) = {
        draw_layout_text,
        draw_layout_loading_text,
        draw_layout_loading_progress_bar,
        draw_layout_error_text
    };

    void set_layout(LayoutType type) {
        layout = type;
        // Clear the screen buffer to redraw
        disp.clear_buf();
        redraw = true;
        unsigned long t = millis();
        // Call the correct init function
        LAYOUT_HANDLERS[static_cast<uint8_t>(layout)](t, true);
    }

    void poll() {
        unsigned long t = millis();
        if (!has_data) {
            // Draw the correct layout
            redraw = LAYOUT_HANDLERS[static_cast<uint8_t>(layout)](t, false) || redraw;
            if (redraw) {
                disp.update();
                redraw = false;
            }
        }
            
        input1.poll(t);
        input2.poll(t);
        status_led.poll(t);
        error_led.poll(t);

        // Exit sleep mode on any input
        if (sleep && (input1.pressed || input2.pressed)) {
            input1.pressed = input2.pressed = false;
            sleep = false;
            disp.write_all(display::MAX7219<DISP_WIDTH>::SHUTDOWN, 1);
        }

        // Increase brightness
        if (input1.pressed) {
            input1.pressed = false;
            if (disp_brightness < 15) {
                disp_brightness ++;
            }
            disp.write_all(display::MAX7219<DISP_WIDTH>::INTENSITY, disp_brightness);
        }
        // Decrease brightness
        if (input2.pressed) {
            input2.pressed = false;
            if (disp_brightness > 0) {
                disp_brightness --;
            }
            disp.write_all(display::MAX7219<DISP_WIDTH>::INTENSITY, disp_brightness);
        }
        // Holding down both buttons for 3s does a soft reset
        if (input1.held && input2.held && input1.held_duration > 3000 && input2.held_duration > 3000) {
            ESP.restart();
        }
        // Re-init the displays
        if (input1.held && !input2.down) {
            input1.held = false;
            for (uint8_t i = 0; i < disp.mod_height; i ++) {
                disp.rows[i].init();
                disp.rows[i].init();
                disp.rows[i].init();
                disp.rows[i].clear();
            }
            disp.update();
            disp.write_all(display::MAX7219<DISP_WIDTH>::INTENSITY, disp_brightness);
            status_led.blink(25, 1);
        }
        if (input2.held && !input1.down) {
            input2.held = false;
            sleep = true;
            disp.write_all(display::MAX7219<DISP_WIDTH>::SHUTDOWN, 0);
            status_led.blink(25, 1);
        }
        // Check for data updates
        if (fb::disp_data_updated) {
            fb::disp_data_updated = false;
            has_data = true;
            status_led.blink(100, 2);
            memcpy(disp.disp_buf, fb::disp_data, sizeof(disp.disp_buf));
            disp.update();
            DEBUG_OUT_LN(F("Display updated"));
            // Automatically exit sleep mode after display update
            if (sleep) {
                sleep = false;
                disp.write_all(display::MAX7219<DISP_WIDTH>::SHUTDOWN, 1);
            }
        }
    }
}

#include "font.h"

#include <pgmspace.h>
#include <stdint.h>

// Right pads binary literal with 0s to make it 1 byte
// sizeof(#x) - 3 subtracts 3 for 0b prefix + null terminator
#define P(x) (x << (8 - (sizeof(#x) - 3)))

namespace font {

    const PROGMEM uint8_t CHAR_SPACE_DATA[] = {
        P(0b00000),
        P(0b00000),
        P(0b00000),
        P(0b00000),
        P(0b00000),
        P(0b00000),
        P(0b00000),
    };

    const PROGMEM uint8_t CHAR_QMARK_DATA[] = {
        P(0b01110),
        P(0b10001),
        P(0b00001),
        P(0b00010),
        P(0b00100),
        P(0b00000),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_A_DATA[] = {
        P(0b00100),
        P(0b01010),
        P(0b10001),
        P(0b10001),
        P(0b11111),
        P(0b10001),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_B_DATA[] = {
        P(0b11110),
        P(0b10001),
        P(0b10001),
        P(0b11110),
        P(0b10001),
        P(0b10001),
        P(0b11110),
    };

    const PROGMEM uint8_t CHAR_C_DATA[] = {
        P(0b01110),
        P(0b10001),
        P(0b10000),
        P(0b10000),
        P(0b10000),
        P(0b10001),
        P(0b01110),
    };

    const PROGMEM uint8_t CHAR_D_DATA[] = {
        P(0b11110),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b11110),
    };

    const PROGMEM uint8_t CHAR_E_DATA[] = {
        P(0b11111),
        P(0b10000),
        P(0b10000),
        P(0b11111),
        P(0b10000),
        P(0b10000),
        P(0b11111),
    };

    const PROGMEM uint8_t CHAR_F_DATA[] = {
        P(0b11111),
        P(0b10000),
        P(0b10000),
        P(0b11111),
        P(0b10000),
        P(0b10000),
        P(0b10000),
    };

    const PROGMEM uint8_t CHAR_G_DATA[] = {
        P(0b01110),
        P(0b10001),
        P(0b10000),
        P(0b10111),
        P(0b10001),
        P(0b10001),
        P(0b01110),
    };

    const PROGMEM uint8_t CHAR_H_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b11111),
        P(0b10001),
        P(0b10001),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_I_DATA[] = {
        P(0b11111),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b11111),
    };

    const PROGMEM uint8_t CHAR_J_DATA[] = {
        P(0b11111),
        P(0b00010),
        P(0b00010),
        P(0b00010),
        P(0b00010),
        P(0b10010),
        P(0b01100),
    };

    const PROGMEM uint8_t CHAR_K_DATA[] = {
        P(0b10001),
        P(0b10010),
        P(0b10100),
        P(0b11000),
        P(0b10100),
        P(0b10010),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_L_DATA[] = {
        P(0b10000),
        P(0b10000),
        P(0b10000),
        P(0b10000),
        P(0b10000),
        P(0b10000),
        P(0b11111),
    };

    const PROGMEM uint8_t CHAR_M_DATA[] = {
        P(0b10001),
        P(0b11011),
        P(0b10101),
        P(0b10101),
        P(0b10001),
        P(0b10001),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_N_DATA[] = {
        P(0b10001),
        P(0b11001),
        P(0b10101),
        P(0b10101),
        P(0b10011),
        P(0b10011),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_O_DATA[] = {
        P(0b01110),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b01110),
    };

    const PROGMEM uint8_t CHAR_P_DATA[] = {
        P(0b11110),
        P(0b10001),
        P(0b10001),
        P(0b11110),
        P(0b10000),
        P(0b10000),
        P(0b10000),
    };

    const PROGMEM uint8_t CHAR_Q_DATA[] = {
        P(0b01110),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10101),
        P(0b10011),
        P(0b01111),
    };

    const PROGMEM uint8_t CHAR_R_DATA[] = {
        P(0b11110),
        P(0b10001),
        P(0b10001),
        P(0b11110),
        P(0b10100),
        P(0b10010),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_S_DATA[] = {
        P(0b01110),
        P(0b10001),
        P(0b10000),
        P(0b01110),
        P(0b00001),
        P(0b10001),
        P(0b01110),
    };

    const PROGMEM uint8_t CHAR_T_DATA[] = {
        P(0b11111),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_U_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b01110),
    };

    const PROGMEM uint8_t CHAR_V_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b01010),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_W_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b10101),
        P(0b10101),
        P(0b01010),
    };

    const PROGMEM uint8_t CHAR_X_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b01010),
        P(0b00100),
        P(0b01010),
        P(0b10001),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_Y_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b01010),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_Z_DATA[] = {
        P(0b11111),
        P(0b00001),
        P(0b00010),
        P(0b00100),
        P(0b01000),
        P(0b10000),
        P(0b11111),
    };

    const PROGMEM Glyph CHAR_UNKNOWN(CHAR_QMARK_DATA, 5, 7);
    
    const PROGMEM Glyph UPPERCASE_CHARS[] = {
        Glyph(CHAR_A_DATA, 5, 7),
        Glyph(CHAR_B_DATA, 5, 7),
        Glyph(CHAR_C_DATA, 5, 7),
        Glyph(CHAR_D_DATA, 5, 7),
        Glyph(CHAR_E_DATA, 5, 7),
        Glyph(CHAR_F_DATA, 5, 7),
        Glyph(CHAR_G_DATA, 5, 7),
        Glyph(CHAR_H_DATA, 5, 7),
        Glyph(CHAR_I_DATA, 5, 7),
        Glyph(CHAR_J_DATA, 5, 7),
        Glyph(CHAR_K_DATA, 5, 7),
        Glyph(CHAR_L_DATA, 5, 7),
        Glyph(CHAR_M_DATA, 5, 7),
        Glyph(CHAR_N_DATA, 5, 7),
        Glyph(CHAR_O_DATA, 5, 7),
        Glyph(CHAR_P_DATA, 5, 7),
        Glyph(CHAR_Q_DATA, 5, 7),
        Glyph(CHAR_R_DATA, 5, 7),
        Glyph(CHAR_S_DATA, 5, 7),
        Glyph(CHAR_T_DATA, 5, 7),
        Glyph(CHAR_U_DATA, 5, 7),
        Glyph(CHAR_V_DATA, 5, 7),
        Glyph(CHAR_W_DATA, 5, 7),
        Glyph(CHAR_X_DATA, 5, 7),
        Glyph(CHAR_Y_DATA, 5, 7),
        Glyph(CHAR_Z_DATA, 5, 7),
    };

    const Glyph& map(char c) {
        if (c >= 'A' && c <= 'Z') {
            return UPPERCASE_CHARS[c - 'A'];
        }
        return CHAR_UNKNOWN;
    }
}

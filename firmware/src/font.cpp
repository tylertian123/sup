#include "font.h"

#include <pgmspace.h>
#include <stdint.h>

// Right pads binary literal with 0s to make it 1 byte
// sizeof(#x) - 3 subtracts 3 for 0b prefix + null terminator
#define P(x) (x << (8 - (sizeof(#x) - 3)))

namespace font {

    const PROGMEM uint8_t CHAR_SPACE_DATA[] = {
        P(0b000),
        P(0b000),
        P(0b000),
        P(0b000),
        P(0b000),
    };

    const PROGMEM uint8_t CHAR_QMARK_DATA[] = {
        P(0b1110),
        P(0b0001),
        P(0b0110),
        P(0b0000),
        P(0b0100),
    };

    const PROGMEM uint8_t CHAR_A_DATA[] = {
        P(0b0110),
        P(0b1001),
        P(0b1111),
        P(0b1001),
        P(0b1001),
    };

    const PROGMEM uint8_t CHAR_B_DATA[] = {
        P(0b1110),
        P(0b1001),
        P(0b1110),
        P(0b1001),
        P(0b1110),
    };

    const PROGMEM uint8_t CHAR_C_DATA[] = {
        P(0b0111),
        P(0b1000),
        P(0b1000),
        P(0b1000),
        P(0b0111),
    };

    const PROGMEM uint8_t CHAR_D_DATA[] = {
        P(0b1110),
        P(0b1001),
        P(0b1001),
        P(0b1001),
        P(0b1110),
    };

    const PROGMEM uint8_t CHAR_E_DATA[] = {
        P(0b1111),
        P(0b1000),
        P(0b1110),
        P(0b1000),
        P(0b1111),
    };

    const PROGMEM uint8_t CHAR_F_DATA[] = {
        P(0b1111),
        P(0b1000),
        P(0b1110),
        P(0b1000),
        P(0b1000),
    };

    const PROGMEM uint8_t CHAR_G_DATA[] = {
        P(0b0111),
        P(0b1000),
        P(0b1011),
        P(0b1001),
        P(0b0111),
    };

    const PROGMEM uint8_t CHAR_H_DATA[] = {
        P(0b1001),
        P(0b1001),
        P(0b1111),
        P(0b1001),
        P(0b1001),
    };

    const PROGMEM uint8_t CHAR_I_DATA[] = {
        P(0b111),
        P(0b010),
        P(0b010),
        P(0b010),
        P(0b111),
    };

    const PROGMEM uint8_t CHAR_J_DATA[] = {
        P(0b111),
        P(0b001),
        P(0b001),
        P(0b101),
        P(0b010),
    };

    const PROGMEM uint8_t CHAR_K_DATA[] = {
        P(0b1001),
        P(0b1010),
        P(0b1100),
        P(0b1010),
        P(0b1001),
    };

    const PROGMEM uint8_t CHAR_L_DATA[] = {
        P(0b100),
        P(0b100),
        P(0b100),
        P(0b100),
        P(0b111),
    };

    const PROGMEM uint8_t CHAR_M_DATA[] = {
        P(0b10001),
        P(0b11011),
        P(0b10101),
        P(0b10001),
        P(0b10001),
    };

    const PROGMEM uint8_t CHAR_N_DATA[] = {
        P(0b1001),
        P(0b1101),
        P(0b1011),
        P(0b1011),
        P(0b1001),
    };

    const PROGMEM uint8_t CHAR_O_DATA[] = {
        P(0b0110),
        P(0b1001),
        P(0b1001),
        P(0b1001),
        P(0b0110),
    };

    const PROGMEM uint8_t CHAR_P_DATA[] = {
        P(0b1110),
        P(0b1001),
        P(0b1110),
        P(0b1000),
        P(0b1000),
    };

    const PROGMEM uint8_t CHAR_Q_DATA[] = {
        P(0b0110),
        P(0b1001),
        P(0b1001),
        P(0b1011),
        P(0b0111),
    };

    const PROGMEM uint8_t CHAR_R_DATA[] = {
        P(0b1110),
        P(0b1001),
        P(0b1110),
        P(0b1010),
        P(0b1001),
    };

    const PROGMEM uint8_t CHAR_S_DATA[] = {
        P(0b0111),
        P(0b1000),
        P(0b0110),
        P(0b0001),
        P(0b1110),
    };

    const PROGMEM uint8_t CHAR_T_DATA[] = {
        P(0b11111),
        P(0b00100),
        P(0b00100),
        P(0b00100),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_U_DATA[] = {
        P(0b1001),
        P(0b1001),
        P(0b1001),
        P(0b1001),
        P(0b0110),
    };

    const PROGMEM uint8_t CHAR_V_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b10001),
        P(0b01010),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_W_DATA[] = {
        P(0b10001),
        P(0b10001),
        P(0b10101),
        P(0b10101),
        P(0b01010),
    };

    const PROGMEM uint8_t CHAR_X_DATA[] = {
        P(0b101),
        P(0b101),
        P(0b010),
        P(0b101),
        P(0b101),
    };

    const PROGMEM uint8_t CHAR_Y_DATA[] = {
        P(0b10001),
        P(0b01010),
        P(0b00100),
        P(0b00100),
        P(0b00100),
    };

    const PROGMEM uint8_t CHAR_Z_DATA[] = {
        P(0b1111),
        P(0b0001),
        P(0b0010),
        P(0b0100),
        P(0b1111),
    };

    const PROGMEM Glyph CHAR_UNKNOWN(CHAR_QMARK_DATA, 4, 5);
    const PROGMEM Glyph CHAR_SPACE(CHAR_SPACE_DATA, 3, 5);
    
    const PROGMEM Glyph UPPERCASE_CHARS[] = {
        Glyph(CHAR_A_DATA, 4, 5),
        Glyph(CHAR_B_DATA, 4, 5),
        Glyph(CHAR_C_DATA, 4, 5),
        Glyph(CHAR_D_DATA, 4, 5),
        Glyph(CHAR_E_DATA, 4, 5),
        Glyph(CHAR_F_DATA, 4, 5),
        Glyph(CHAR_G_DATA, 4, 5),
        Glyph(CHAR_H_DATA, 4, 5),
        Glyph(CHAR_I_DATA, 3, 5),
        Glyph(CHAR_J_DATA, 3, 5),
        Glyph(CHAR_K_DATA, 4, 5),
        Glyph(CHAR_L_DATA, 3, 5),
        Glyph(CHAR_M_DATA, 5, 5),
        Glyph(CHAR_N_DATA, 4, 5),
        Glyph(CHAR_O_DATA, 4, 5),
        Glyph(CHAR_P_DATA, 4, 5),
        Glyph(CHAR_Q_DATA, 4, 5),
        Glyph(CHAR_R_DATA, 4, 5),
        Glyph(CHAR_S_DATA, 4, 5),
        Glyph(CHAR_T_DATA, 5, 5),
        Glyph(CHAR_U_DATA, 4, 5),
        Glyph(CHAR_V_DATA, 5, 5),
        Glyph(CHAR_W_DATA, 5, 5),
        Glyph(CHAR_X_DATA, 3, 5),
        Glyph(CHAR_Y_DATA, 5, 5),
        Glyph(CHAR_Z_DATA, 4, 5),
    };

    const Glyph& map(char c) {
        if (c >= 'A' && c <= 'Z') {
            return UPPERCASE_CHARS[c - 'A'];
        }
        // Font size is too small for lowercase letters
        if (c >= 'a' && c <= 'z') {
            return UPPERCASE_CHARS[c - 'a'];
        }
        switch (c) {
        case ' ':
            return CHAR_SPACE;
        case '?':
        default:
            return CHAR_UNKNOWN;
        }
    }
}

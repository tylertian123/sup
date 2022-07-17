#include "graphics.h"

#include <pgmspace.h>
#include <stdint.h>

namespace graphics {

    const PROGMEM uint8_t CHAR_SPACE_DATA[] = {
        _PAD(0b000),
        _PAD(0b000),
        _PAD(0b000),
        _PAD(0b000),
        _PAD(0b000),
    };

    const PROGMEM uint8_t CHAR_QMARK_DATA[] = {
        _PAD(0b1110),
        _PAD(0b0001),
        _PAD(0b0110),
        _PAD(0b0000),
        _PAD(0b0100),
    };

    const PROGMEM uint8_t CHAR_EXMARK_DATA[] = {
        _PAD(0b1),
        _PAD(0b1),
        _PAD(0b1),
        _PAD(0b0),
        _PAD(0b1),
    };

    const PROGMEM uint8_t CHAR_PCT_DATA[] = {
        _PAD(0b11001),
        _PAD(0b11010),
        _PAD(0b00100),
        _PAD(0b01011),
        _PAD(0b10011),
    };

    const PROGMEM uint8_t CHAR_A_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b1111),
        _PAD(0b1001),
        _PAD(0b1001),
    };

    const PROGMEM uint8_t CHAR_B_DATA[] = {
        _PAD(0b1110),
        _PAD(0b1001),
        _PAD(0b1110),
        _PAD(0b1001),
        _PAD(0b1110),
    };

    const PROGMEM uint8_t CHAR_C_DATA[] = {
        _PAD(0b0111),
        _PAD(0b1000),
        _PAD(0b1000),
        _PAD(0b1000),
        _PAD(0b0111),
    };

    const PROGMEM uint8_t CHAR_D_DATA[] = {
        _PAD(0b1110),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1110),
    };

    const PROGMEM uint8_t CHAR_E_DATA[] = {
        _PAD(0b111),
        _PAD(0b100),
        _PAD(0b111),
        _PAD(0b1000),
        _PAD(0b111),
    };

    const PROGMEM uint8_t CHAR_F_DATA[] = {
        _PAD(0b111),
        _PAD(0b100),
        _PAD(0b111),
        _PAD(0b100),
        _PAD(0b100),
    };

    const PROGMEM uint8_t CHAR_G_DATA[] = {
        _PAD(0b0111),
        _PAD(0b1000),
        _PAD(0b1011),
        _PAD(0b1001),
        _PAD(0b0111),
    };

    const PROGMEM uint8_t CHAR_H_DATA[] = {
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1111),
        _PAD(0b1001),
        _PAD(0b1001),
    };

    const PROGMEM uint8_t CHAR_I_DATA[] = {
        _PAD(0b111),
        _PAD(0b010),
        _PAD(0b010),
        _PAD(0b010),
        _PAD(0b111),
    };

    const PROGMEM uint8_t CHAR_J_DATA[] = {
        _PAD(0b111),
        _PAD(0b001),
        _PAD(0b001),
        _PAD(0b101),
        _PAD(0b010),
    };

    const PROGMEM uint8_t CHAR_K_DATA[] = {
        _PAD(0b1001),
        _PAD(0b1010),
        _PAD(0b1100),
        _PAD(0b1010),
        _PAD(0b1001),
    };

    const PROGMEM uint8_t CHAR_L_DATA[] = {
        _PAD(0b100),
        _PAD(0b100),
        _PAD(0b100),
        _PAD(0b100),
        _PAD(0b111),
    };

    const PROGMEM uint8_t CHAR_M_DATA[] = {
        _PAD(0b10001),
        _PAD(0b11011),
        _PAD(0b10101),
        _PAD(0b10001),
        _PAD(0b10001),
    };

    const PROGMEM uint8_t CHAR_N_DATA[] = {
        _PAD(0b1001),
        _PAD(0b1101),
        _PAD(0b1011),
        _PAD(0b1011),
        _PAD(0b1001),
    };

    const PROGMEM uint8_t CHAR_O_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b0110),
    };

    const PROGMEM uint8_t CHAR_P_DATA[] = {
        _PAD(0b1110),
        _PAD(0b1001),
        _PAD(0b1110),
        _PAD(0b1000),
        _PAD(0b1000),
    };

    const PROGMEM uint8_t CHAR_Q_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1011),
        _PAD(0b0111),
    };

    const PROGMEM uint8_t CHAR_R_DATA[] = {
        _PAD(0b1110),
        _PAD(0b1001),
        _PAD(0b1110),
        _PAD(0b1010),
        _PAD(0b1001),
    };

    const PROGMEM uint8_t CHAR_S_DATA[] = {
        _PAD(0b0111),
        _PAD(0b1000),
        _PAD(0b0110),
        _PAD(0b0001),
        _PAD(0b1110),
    };

    const PROGMEM uint8_t CHAR_T_DATA[] = {
        _PAD(0b111),
        _PAD(0b010),
        _PAD(0b010),
        _PAD(0b010),
        _PAD(0b010),
    };

    const PROGMEM uint8_t CHAR_U_DATA[] = {
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b0110),
    };

    const PROGMEM uint8_t CHAR_V_DATA[] = {
        _PAD(0b10001),
        _PAD(0b10001),
        _PAD(0b10001),
        _PAD(0b01010),
        _PAD(0b00100),
    };

    const PROGMEM uint8_t CHAR_W_DATA[] = {
        _PAD(0b10001),
        _PAD(0b10001),
        _PAD(0b10101),
        _PAD(0b10101),
        _PAD(0b01010),
    };

    const PROGMEM uint8_t CHAR_X_DATA[] = {
        _PAD(0b101),
        _PAD(0b101),
        _PAD(0b010),
        _PAD(0b101),
        _PAD(0b101),
    };

    const PROGMEM uint8_t CHAR_Y_DATA[] = {
        _PAD(0b10001),
        _PAD(0b01010),
        _PAD(0b00100),
        _PAD(0b00100),
        _PAD(0b00100),
    };

    const PROGMEM uint8_t CHAR_Z_DATA[] = {
        _PAD(0b1111),
        _PAD(0b0001),
        _PAD(0b0010),
        _PAD(0b0100),
        _PAD(0b1111),
    };

    const PROGMEM uint8_t CHAR_0_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b1001),
        _PAD(0b0110),
    };

    const PROGMEM uint8_t CHAR_1_DATA[] = {
        _PAD(0b010),
        _PAD(0b110),
        _PAD(0b010),
        _PAD(0b010),
        _PAD(0b111),
    };

    const PROGMEM uint8_t CHAR_2_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b0010),
        _PAD(0b0100),
        _PAD(0b1111),
    };

    const PROGMEM uint8_t CHAR_3_DATA[] = {
        _PAD(0b1110),
        _PAD(0b0001),
        _PAD(0b0110),
        _PAD(0b0001),
        _PAD(0b1110),
    };

    const PROGMEM uint8_t CHAR_4_DATA[] = {
        _PAD(0b0010),
        _PAD(0b0110),
        _PAD(0b1010),
        _PAD(0b1111),
        _PAD(0b0010),
    };

    const PROGMEM uint8_t CHAR_5_DATA[] = {
        _PAD(0b1111),
        _PAD(0b1000),
        _PAD(0b1110),
        _PAD(0b0001),
        _PAD(0b1110),
    };

    const PROGMEM uint8_t CHAR_6_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1000),
        _PAD(0b1110),
        _PAD(0b1001),
        _PAD(0b0110),
    };

    const PROGMEM uint8_t CHAR_7_DATA[] = {
        _PAD(0b1111),
        _PAD(0b0001),
        _PAD(0b0010),
        _PAD(0b0010),
        _PAD(0b0010),
    };

    const PROGMEM uint8_t CHAR_8_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b0110),
    };

    const PROGMEM uint8_t CHAR_9_DATA[] = {
        _PAD(0b0110),
        _PAD(0b1001),
        _PAD(0b0111),
        _PAD(0b0001),
        _PAD(0b0110),
    };

    const PROGMEM Glyph CHAR_QMARK(CHAR_QMARK_DATA, 4, 5);
    const PROGMEM Glyph CHAR_EXMARK(CHAR_EXMARK_DATA, 1, 5);
    const PROGMEM Glyph CHAR_SPACE(CHAR_SPACE_DATA, 1, 5);
    const PROGMEM Glyph CHAR_PERCENT(CHAR_PCT_DATA, 5, 5);
    
    const PROGMEM Glyph UPPERCASE_CHARS[] = {
        Glyph(CHAR_A_DATA, 4, 5),
        Glyph(CHAR_B_DATA, 4, 5),
        Glyph(CHAR_C_DATA, 4, 5),
        Glyph(CHAR_D_DATA, 4, 5),
        Glyph(CHAR_E_DATA, 3, 5),
        Glyph(CHAR_F_DATA, 3, 5),
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
        Glyph(CHAR_T_DATA, 3, 5),
        Glyph(CHAR_U_DATA, 4, 5),
        Glyph(CHAR_V_DATA, 5, 5),
        Glyph(CHAR_W_DATA, 5, 5),
        Glyph(CHAR_X_DATA, 3, 5),
        Glyph(CHAR_Y_DATA, 5, 5),
        Glyph(CHAR_Z_DATA, 4, 5),
    };

    const PROGMEM Glyph NUMBER_CHARS[] = {
        Glyph(CHAR_0_DATA, 4, 5),
        Glyph(CHAR_1_DATA, 3, 5),
        Glyph(CHAR_2_DATA, 4, 5),
        Glyph(CHAR_3_DATA, 4, 5),
        Glyph(CHAR_4_DATA, 4, 5),
        Glyph(CHAR_5_DATA, 4, 5),
        Glyph(CHAR_6_DATA, 4, 5),
        Glyph(CHAR_7_DATA, 4, 5),
        Glyph(CHAR_8_DATA, 4, 5),
        Glyph(CHAR_9_DATA, 4, 5),
    };

    const Glyph& map_char(char c) {
        if (c >= 'A' && c <= 'Z') {
            return UPPERCASE_CHARS[c - 'A'];
        }
        // Lowercase and uppercase letters look the same
        if (c >= 'a' && c <= 'z') {
            return UPPERCASE_CHARS[c - 'a'];
        }
        if (c >= '0' && c <= '9') {
            return NUMBER_CHARS[c - '0'];
        }
        switch (c) {
        case ' ':
            return CHAR_SPACE;
        case '!':
            return CHAR_EXMARK;
        case '%':
            return CHAR_PERCENT;
        case '?':
        default:
            return CHAR_QMARK;
        }
    }
}

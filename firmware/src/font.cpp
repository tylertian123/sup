#include "font.h"

#include <pgmspace.h>
#include <stdint.h>

namespace font {

    const PROGMEM uint8_t CHAR_A_DATA[] = {
        0b00100 << 3,
        0b01010 << 3,
        0b10001 << 3,
        0b10001 << 3,
        0b11111 << 3,
        0b10001 << 3,
        0b10001 << 3,
    };

    const PROGMEM Glyph CHAR_A(CHAR_A_DATA, 5, 7);
}

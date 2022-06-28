#pragma once

#include <stdint.h>

namespace font {
    class Glyph {
    public:
        const uint8_t *data;
        uint8_t width;
        uint8_t height;
        uint8_t width_bytes;
        
        constexpr Glyph(const uint8_t *data, uint8_t width, uint8_t height)
            // Width is divided by 8 and rounded up to get byte width
            : data(data), width(width), height(height), width_bytes((width - 1) / 8 + 1) {}
    };

    const Glyph& map(char c);
}

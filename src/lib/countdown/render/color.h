#pragma once

#include <cstdint>

namespace countdown { namespace render {
    struct ColorRGB
    {
        static const ColorRGB BLACK;
        static const ColorRGB WHITE;

        ColorRGB(uint8_t r, uint8_t g, uint8_t b)
            : R(r), G(g), B(b) {}

        uint8_t R;
        uint8_t G;
        uint8_t B;
    };

}}

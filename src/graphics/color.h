#pragma once
#include "SFML/Graphics.hpp"
#include <cstdint>

namespace graphics {
class Color {
public:
    Color(const sf::Color &color);
    Color(const uint8_t &r, const uint8_t &g, const uint8_t &b, const uint8_t &a = 255);
    Color(const uint32_t &hex);
    Color(const uint32_t &hex, const uint8_t &a);

    static Color fromHex(const std::string &hex);

    Color &operator=(const Color &color);

    sf::Color toSF() const;

    const uint8_t r;   // red
    const uint8_t g;   // green
    const uint8_t b;   // blue
    const uint8_t a;   // alpha

    static const Color Transparent;
    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;
    static const Color Orange;
    static const Color Gray;
    static const Color LightGray;
    static const Color DarkGray;
    static const Color Brown;
    static const Color LightBrown;
    static const Color DarkBrown;
    static const Color Purple;
};

}   // namespace graphics

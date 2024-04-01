#include "color.h"
#include <cassert>
#include <sstream>

namespace graphics {

Color::Color(const sf::Color &color) : r(color.r), g(color.g), b(color.b), a(color.a) {}

Color::Color(const uint8_t &r, const uint8_t &g, const uint8_t &b, const uint8_t &a) : r(r), g(g), b(b), a(a) {};
Color::Color(const uint32_t &hex) : r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF), a(255) {};
Color::Color(const uint32_t &hex, const uint8_t &a) :
    r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF), a(a) {};

Color Color::fromHex(const std::string &hex) {
    uint32_t hexValue;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> hexValue;
    return Color(hexValue);
}

sf::Color Color::toSF() const {
    return {r, g, b, a};
}

Color &Color::operator=(const Color &color) {
    assert(this != &color);
    const_cast<uint8_t &>(r) = color.r;
    const_cast<uint8_t &>(g) = color.g;
    const_cast<uint8_t &>(b) = color.b;
    const_cast<uint8_t &>(a) = color.a;
    return *this;
}

const Color Color::Transparent = Color(0, 0, 0, 0);
const Color Color::Black = Color(0, 0, 0);
const Color Color::White = Color(255, 255, 255);
const Color Color::Red = Color(255, 0, 0);
const Color Color::Green = Color(0, 255, 0);
const Color Color::Blue = Color(0, 0, 255);
const Color Color::Yellow = Color(255, 255, 0);
const Color Color::Magenta = Color(255, 0, 255);
const Color Color::Cyan = Color(0, 255, 255);
const Color Color::Orange = Color(255, 165, 0);
const Color Color::Gray = Color(128, 128, 128);
const Color Color::LightGray = Color(192, 192, 192);
const Color Color::DarkGray = Color(64, 64, 64);
const Color Color::Brown = Color(165, 42, 42);
const Color Color::LightBrown = Color(205, 133, 63);
const Color Color::DarkBrown = Color(139, 69, 19);
const Color Color::Purple = Color(128, 0, 128);

}   // namespace graphics

#include "text.h"
#include "vector2d/vector2d.h"
#include <map>

namespace graphics {

static std::map<std::string, sf::Font> fonts;

static sf::Font &getFont(const std::string &name) {
    if (fonts.find(name) == fonts.end())
        fonts[name].loadFromFile(cfg::FONTS_PATH + name + ".ttf");
    return fonts[name];
}

Text::Text(const std::string &text) {
    m_text.setString(text);
    m_text.setFont(getFont(cfg::DEFAULT_FONT));
    m_text.setCharacterSize(cfg::DIALOGUE_FONT_SIZE);
    m_text.setFillColor(sf::Color::White);
    m_text.setOutlineColor(sf::Color::Black);
}

Text &Text::setString(const std::string &text) {
    m_text.setString(text);
    return *this;
}

Text &Text::setFillColor(const Color &color) {
    m_text.setFillColor(color.toSF());
    return *this;
}

Text &Text::setOutlineColor(const Color &color) {
    m_text.setOutlineColor(color.toSF());
    return *this;
}

Text &Text::setOutlineThickness(const float &thickness) {
    m_text.setOutlineThickness(thickness);
    return *this;
}

Text &Text::setStyle(const sf::Uint32 style) {
    m_text.setStyle(style);
    return *this;
}

Text &Text::setRotation(const float &angle) {
    m_text.setRotation(angle);
    return *this;
}

Text &Text::setLetterSpacing(const float &spacing_factor) {
    m_text.setLetterSpacing(spacing_factor);
    return *this;
}

Text &Text::setScale(const float &factorX, const float &factorY) {
    m_text.setScale(factorX, factorY);
    return *this;
}

Text &Text::setFontSize(const int &size) {
    m_text.setCharacterSize(size);
    return *this;
}

Text &Text::setFont(const std::string &font) {
    m_text.setFont(getFont(font));
    return *this;
}

Text &Text::setOrigin(const Origin &origin) {
    Vector2d size = getSize();
    m_origin = origin;
    switch (origin) {
        case Origin::TOP_LEFT:
            m_text.setOrigin(0, 0);
            break;
        case Origin::TOP_CENTER:
            m_text.setOrigin(size.x / 2, 0);
            break;
        case Origin::TOP_RIGHT:
            m_text.setOrigin(size.x, 0);
            break;
        case Origin::CENTER_LEFT:
            m_text.setOrigin(0, size.y / 2);
            break;
        case Origin::CENTER:
            m_text.setOrigin(size.x / 2, size.y / 2);
            break;
        case Origin::CENTER_RIGHT:
            m_text.setOrigin(size.x, size.y / 2);
            break;
        case Origin::BOTTOM_LEFT:
            m_text.setOrigin(0, size.y);
            break;
        case Origin::BOTTOM_CENTER:
            m_text.setOrigin(size.x / 2, size.y);
            break;
        case Origin::BOTTOM_RIGHT:
            m_text.setOrigin(size.x, size.y);
            break;
    }
    return *this;
}

Text &Text::setPosition(const Vector2d &position) {
    m_text.setPosition(position.toSF());
    return *this;
}

const Text::Origin Text::getOrigin() const {
    return m_origin;
}

const Vector2d Text::getPosition() const {
    return m_text.getPosition();
}

const Vector2d Text::getSize() const {
    return m_text.getLocalBounds().getSize();
}

void Text::draw(const Renderer &renderer) const {
    renderer.getWindow().draw(m_text);
}

}   // namespace graphics

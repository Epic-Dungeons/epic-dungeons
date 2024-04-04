#pragma once
#include "SFML/Graphics/Color.hpp"
#include "graphics/color.h"
#include "graphics/renderer.h"
#include "static_data/game_config.h"
#include "vector2d/vector2d.h"

namespace graphics {
class Text : public Drawable {
public:
    Text(const std::string &text);

    enum Origin {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        CENTER_LEFT,
        CENTER,
        CENTER_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER,
        BOTTOM_RIGHT
    };

    enum Style {
        Regular = 0,             //!< Regular characters, no style
        Bold = 1 << 0,           //!< Bold characters
        Italic = 1 << 1,         //!< Italic characters
        Underlined = 1 << 2,     //!< Underlined characters
        StrikeThrough = 1 << 3   //!< Strike through characters
    };

    Text &setString(const std::string &text);
    Text &setFillColor(const Color &color);
    Text &setOutlineColor(const Color &color);
    Text &setOutlineThickness(const float &thickness);
    Text &setStyle(const sf::Uint32 style);
    Text &setRotation(const float &angle);
    Text &setPosition(const Vector2d &position);
    Text &setLetterSpacing(const float &spacingFactor);
    Text &setScale(const float &factorX, const float &factorY);
    Text &setFontSize(const int &size);
    Text &setFont(const std::string &font);
    Text &setOrigin(const Origin &origin);

    const Vector2d getSize() const;
    const Vector2d getPosition() const;
    const Origin getOrigin() const;

protected:
    virtual void draw(const Renderer &renderer) const override;
    mutable sf::Text m_text;
    Origin m_origin = Origin::TOP_LEFT;
};
}   // namespace graphics

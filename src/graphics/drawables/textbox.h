#pragma once

#include "rectangle.h"
#include "text.h"

namespace graphics {
class TextBox : protected Rectangle, protected Text {
public:
    TextBox(const Vector2d &position, const Vector2d &size, const Color &color = 0x000000FF);
    TextBox(const float &x, const float &y, const float &w, const float &h, const Color &color = 0x000000FF);

    enum class Alignment { LEFT, CENTER, RIGHT };

    TextBox &setPosition(const Vector2d &position);
    TextBox &setSize(const Vector2d &size);
    TextBox &setBoxFillColor(const Color &color);
    TextBox &setBoxOutlineColor(const Color &color);
    TextBox &setBoxOutlineThickness(const float &thickness);
    TextBox &setTextFillColor(const Color &color);
    TextBox &setTextOutlineColor(const Color &color);
    TextBox &setTextOutlineThickness(const float &thickness);
    TextBox &setOrigin(const Origin &origin);
    TextBox &setAlignment(const Alignment &alignment);
    TextBox &setString(const std::string &text);
    TextBox &setStyle(const sf::Uint32 style);

    virtual void draw(const Renderer &renderer) const override;

protected:
    Alignment m_alignment = Alignment::LEFT;
    std::string m_line;
};
}   // namespace graphics
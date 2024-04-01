
#pragma once
#include "graphics/color.h"
#include "graphics/renderer.h"
#include "vector2d/vector2d.h"
#include <SFML/Graphics.hpp>

namespace graphics {
class Rectangle : public Drawable {
public:
    Rectangle(const Vector2d &position, const Vector2d &size, const Color &color = 0x000000FF);
    Rectangle(const float &x, const float &y, const float &w, const float &h, const Color &color = 0x000000FF);

    Rectangle &setPosition(const Vector2d &position);
    Rectangle &setSize(const Vector2d &size);
    Rectangle &setColor(const Color &color);
    Rectangle &setOutlineColor(const Color &color);
    Rectangle &setOutlineThickness(const float &thickness);
    Rectangle &setRotation(const float &angle);
    Rectangle &setScale(const float &factorX, const float &factorY);
    Rectangle &setScale(const float &ratio);
    Rectangle &setScale(const Vector2d &scale);

    const Vector2d getSize() const;
    const Vector2d getPosition() const;
    const Vector2d getScale() const;

protected:
    virtual void draw(const Renderer &renderer) const override;

private:
    mutable sf::RectangleShape m_rectangle;
};

}   // namespace graphics

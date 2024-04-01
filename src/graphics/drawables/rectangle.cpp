
#include "rectangle.h"
#include "vector2d/vector2d.h"

namespace graphics {

Rectangle::Rectangle(const Vector2d &position, const Vector2d &size, const Color &color) : Drawable() {
    m_rectangle.setPosition(position.toSF());
    m_rectangle.setSize(size.toSF());
    m_rectangle.setFillColor(color.toSF());
};

Rectangle::Rectangle(const float &x, const float &y, const float &w, const float &h, const Color &color) : Drawable() {
    m_rectangle.setPosition(x, y);
    m_rectangle.setSize({w, h});
    m_rectangle.setFillColor(color.toSF());
};

Rectangle &Rectangle::setPosition(const Vector2d &position) {
    m_rectangle.setPosition(position.toSF());
    return *this;
}

Rectangle &Rectangle::setColor(const Color &color) {
    m_rectangle.setFillColor(color.toSF());
    return *this;
}

Rectangle &Rectangle::setSize(const Vector2d &size) {
    m_rectangle.setSize(size.toSF());
    return *this;
}

Rectangle &Rectangle::setOutlineColor(const Color &color) {
    m_rectangle.setOutlineColor(color.toSF());
    return *this;
}

Rectangle &Rectangle::setOutlineThickness(const float &thickness) {
    m_rectangle.setOutlineThickness(thickness);
    return *this;
}

Rectangle &Rectangle::setRotation(const float &angle) {
    m_rectangle.setRotation(angle);
    return *this;
}

Rectangle &Rectangle::setScale(const float &factorX, const float &factorY) {
    m_rectangle.setScale(factorX, factorY);
    return *this;
}

Rectangle &Rectangle::setScale(const float &ratio) {
    m_rectangle.setScale(ratio, ratio);
    return *this;
}

Rectangle &Rectangle::setScale(const Vector2d &scale) {
    m_rectangle.setScale(scale.toSF());
    return *this;
}

const Vector2d Rectangle::getSize() const {
    return m_rectangle.getSize();
}

const Vector2d Rectangle::getPosition() const {
    return m_rectangle.getPosition();
}

const Vector2d Rectangle::getScale() const {
    return m_rectangle.getScale();
}

void Rectangle::draw(const Renderer &renderer) const {
    renderer.getWindow().draw(m_rectangle);
}

}   // namespace graphics

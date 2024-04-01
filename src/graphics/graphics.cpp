#include "graphics.h"
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <sstream>

namespace graphics {

Renderer::Renderer(sf::RenderWindow &window) : window(window) {}

sf::RenderWindow &Renderer::getWindow() const {
    return window;
}

void Renderer::clear() {
    window.clear();
}

void Renderer::display() {
    window.display();
}

void Renderer::close() {
    window.close();
    active = false;
}

bool Renderer::isActive() const {
    return active;
}

void Renderer::open() {
    active = true;
}

void Renderer::draw(const Drawable &drawable) const {
    drawable.draw(*this);
}

void Renderer::draw(const sf::Drawable &drawable) const {
    window.draw(drawable);
}

void Renderer::draw(const std::shared_ptr<Drawable> &drawable) const {
    drawable->draw(*this);
}

}   // namespace graphics

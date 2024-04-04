#include "graphics.h"
#include "logging/logger.h"
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
    // m_items.clear();
}

void Renderer::clearUntil(const Drawable &drawable) {
    // clear from back to front
    // for (auto it = m_items.rbegin(); it != m_items.rend(); ++it) {
    //     if (*it == &drawable) {
    //         m_items.erase(m_items.begin(), it.base());
    //         return;
    //     }
    // }
    // if not found, clear all
    // m_items.clear();
}

void Renderer::display() const {
    displayFlag = true;
}

void Renderer::trueDisplay() {
    // window.clear();
    // for (const auto &item : m_items) {
    //     logging::debug("Drawing item: {}", item->objectId);
    //     item->draw(*this);
    //     logging::debug("Item {} drawn", item->objectId);
    // }
    if (displayFlag) {
        window.display();
    }
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

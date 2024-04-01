#pragma once
#include "drawables/drawable.h"
#include "keyboard/keyboard.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace graphics {
class Drawable;

class Renderer {
public:
    // constructor
    Renderer(sf::RenderWindow &window);
    friend class Drawable;

    void display();
    void clear();

    sf::RenderWindow &getWindow() const;
    void draw(const Drawable &drawable) const;
    void draw(const sf::Drawable &drawable) const;
    void draw(const std::shared_ptr<Drawable> &drawable) const;
    void close();
    bool isActive() const;
    void open();

private:
    sf::RenderWindow &window;
    bool active;
};

}   // namespace graphics

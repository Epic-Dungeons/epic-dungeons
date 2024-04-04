#include "textbox.h"

namespace graphics {

TextBox::TextBox(const Vector2d &position, const Vector2d &size, const Color &color) :
    Rectangle(position, size, color), Text("") {}

TextBox::TextBox(const float &x, const float &y, const float &w, const float &h, const Color &color) :
    Rectangle(x, y, w, h, color), Text("") {}

TextBox &TextBox::setOrigin(const Origin &origin) {
    Text::setOrigin(origin);
    return *this;
}

TextBox &TextBox::setString(const std::string &text) {
    m_line = text;
    return *this;
}

TextBox &TextBox::setStyle(const sf::Uint32 style) {
    Text::setStyle(style);
    return *this;
}

TextBox &TextBox::setPosition(const Vector2d &position) {
    Rectangle::setPosition(position);
    return *this;
}

TextBox &TextBox::setSize(const Vector2d &size) {
    Rectangle::setSize(size);
    return *this;
}

TextBox &TextBox::setBoxFillColor(const Color &color) {
    Rectangle::setColor(color);
    return *this;
}

TextBox &TextBox::setBoxOutlineColor(const Color &color) {
    Rectangle::setOutlineColor(color);
    return *this;
}

TextBox &TextBox::setBoxOutlineThickness(const float &thickness) {
    Rectangle::setOutlineThickness(thickness);
    return *this;
}

TextBox &TextBox::setTextFillColor(const Color &color) {
    Text::setFillColor(color);
    return *this;
}

TextBox &TextBox::setTextOutlineColor(const Color &color) {
    Text::setOutlineColor(color);
    return *this;
}

TextBox &TextBox::setTextOutlineThickness(const float &thickness) {
    Text::setOutlineThickness(thickness);
    return *this;
}

TextBox &TextBox::setAlignment(const Alignment &alignment) {
    m_alignment = alignment;
    return *this;
}

namespace {
std::vector<std::string> split(const std::string &text, const char &delimiter) {
    std::vector<std::string> words;
    std::string word;
    for (auto &c : text) {
        if (c == delimiter) {
            words.push_back(word);
            word.clear();
        } else {
            word += c;
        }
    }
    words.push_back(word);
    return words;
}
}   // namespace

void TextBox::draw(const Renderer &renderer) const {
    // TODO: Implement TextBox::draw
}

}   // namespace graphics

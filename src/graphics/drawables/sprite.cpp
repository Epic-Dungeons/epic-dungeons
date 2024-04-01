#include "sprite.h"
#include "logging/logger.h"
#include <stdexcept>

namespace graphics {

static std::map<std::string, std::shared_ptr<sf::Texture>> textures;

static std::shared_ptr<sf::Texture> getTexture(const std::string &path) {
    if (textures.find(path) == textures.end()) {
        textures[path] = std::make_shared<sf::Texture>();
        if (!textures[path]->loadFromFile(path)) {
            logging::error("Error loading texture from file: " + path);
            throw std::runtime_error("Error loading texture from file: " + path);
        }
        logging::debug("Loaded texture " + path);
        textures[path]->setSmooth(true);   // enable smooth texture
    }
    return textures[path];
}

Sprite::Sprite(const std::string &path) {
    m_path = cfg::SPRITES_PATH + path;
    m_texture = getTexture(m_path);
    m_sprite.setTexture(*m_texture);
    m_origin = Origin::TOP_LEFT;
    logging::debug("Loaded sprite " + m_path);
}

Sprite &Sprite::setColor(const graphics::Color &color) {
    m_sprite.setColor(color.toSF());
    return *this;
}

Sprite &Sprite::setScale(const float &factorX, const float &factorY) {
    m_sprite.setScale(factorX, factorY);
    return *this;
}

const Vector2d Sprite::getScale() const {
    return m_sprite.getScale();
}

Sprite &Sprite::setScale(const float &ratio) {
    m_sprite.setScale(ratio, ratio);
    return *this;
}

Sprite &Sprite::setRotation(const float &rotation_angle) {
    m_sprite.setRotation(rotation_angle);
    return *this;
}

const Vector2d Sprite::getSize() const {
    Vector2d size = m_texture->getSize();
    Vector2d scale = m_sprite.getScale();
    return {size.x * scale.x, size.y * scale.y};
}

Sprite &Sprite::toSizeX(const float &x) {
    setScale(x / m_texture->getSize().x);
    return *this;
}

Sprite &Sprite::toSizeY(const float &y) {
    setScale(y / m_texture->getSize().y);
    return *this;
}

Sprite &Sprite::flipX() {
    setScale(-m_sprite.getScale().x, m_sprite.getScale().y);
    return *this;
}

Sprite &Sprite::flipY() {
    setScale(m_sprite.getScale().x, -m_sprite.getScale().y);
    return *this;
}

Sprite &Sprite::setFlip(const bool &flipX, const bool &flipY) {
    Vector2d scale = m_sprite.getScale();
    bool flip_x = (scale.x < 0) ^ flipX;
    bool flip_y = (scale.y < 0) ^ flipY;
    setScale(flip_x ? -scale.x : scale.x, flip_y ? -scale.y : scale.y);
    return *this;
}

Sprite &Sprite::toSize(const float &x, const float &y) {
    setScale(x / m_texture->getSize().x, y / m_texture->getSize().y);
    return *this;
}

Sprite &Sprite::setPosition(const Vector2d &position) {
    m_sprite.setPosition(position.toSF());
    return *this;
}

const Vector2d Sprite::getPosition() const {
    return m_sprite.getPosition();
}

void Sprite::draw(const Renderer &renderer) const {
    renderer.getWindow().draw(m_sprite);
}

Sprite &Sprite::setOrigin(const Origin &origin) {
    Vector2d size = {m_texture->getSize().x, m_texture->getSize().y};
    m_origin = origin;
    switch (origin) {
        case Origin::TOP_LEFT:
            m_sprite.setOrigin(0, 0);
            break;
        case Origin::TOP_CENTER:
            m_sprite.setOrigin(size.x / 2, 0);
            break;
        case Origin::TOP_RIGHT:
            m_sprite.setOrigin(size.x, 0);
            break;
        case Origin::CENTER_LEFT:
            m_sprite.setOrigin(0, size.y / 2);
            break;
        case Origin::CENTER:
            m_sprite.setOrigin(size.x / 2, size.y / 2);
            break;
        case Origin::CENTER_RIGHT:
            m_sprite.setOrigin(size.x, size.y / 2);
            break;
        case Origin::BOTTOM_LEFT:
            m_sprite.setOrigin(0, size.y);
            break;
        case Origin::BOTTOM_CENTER:
            m_sprite.setOrigin(size.x / 2, size.y);
            break;
        case Origin::BOTTOM_RIGHT:
            m_sprite.setOrigin(size.x, size.y);
            break;
    }
    return *this;
}

const Sprite::Origin Sprite::getOrigin() const {
    return m_origin;
}

}   // namespace graphics

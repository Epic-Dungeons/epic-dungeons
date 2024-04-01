#pragma once
#include "graphics/color.h"
#include "graphics/renderer.h"
#include "static_data/game_config.h"
#include "vector2d/vector2d.h"
#include <memory>
#include <string>

namespace graphics {
class Sprite : public Drawable {
public:
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

    Sprite(const std::string &path);
    Sprite &setColor(const graphics::Color &color);
    Sprite &setScale(const float &factorX, const float &factorY);
    Sprite &setScale(const float &ratio);
    Sprite &setRotation(const float &angle);
    Sprite &toSizeY(const float &y);
    Sprite &toSizeX(const float &x);
    Sprite &toSize(const float &x, const float &y);
    Sprite &setPosition(const Vector2d &position);
    Sprite &setOrigin(const Origin &origin);
    Sprite &flipX();
    Sprite &flipY();
    Sprite &setFlip(const bool &flipX, const bool &flipY);
    const Vector2d getSize() const;
    const Vector2d getScale() const;
    const Vector2d getPosition() const;
    const Origin getOrigin() const;

    static const std::shared_ptr<graphics::Sprite> load(const std::string &path) {
        return std::make_shared<graphics::Sprite>(path);
    }

protected:
    std::string m_path;
    mutable sf::Sprite m_sprite;
    std::shared_ptr<sf::Texture> m_texture;
    Origin m_origin;
    virtual void draw(const Renderer &renderer) const override;
};

typedef std::shared_ptr<graphics::Sprite> SpritePtr;

}   // namespace graphics

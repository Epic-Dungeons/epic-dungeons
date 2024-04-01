#pragma once
#include "graphics/color.h"
#include "graphics/renderer.h"
#include "sprite.h"
#include "static_data/game_config.h"
#include "vector2d/vector2d.h"
#include <chrono>
#include <iostream>
#include <memory>

namespace graphics {

class Animation;

typedef std::shared_ptr<Animation> AnimationPtr;

class Animation : public Sprite {
public:
    Animation(const std::string &path);
    const uint32_t getFramesCount() const;
    std::shared_ptr<sf::Texture> getFrame() const;

    static const AnimationPtr load(const std::string &path) {
        return std::make_shared<Animation>(path);
    }

protected:
    void draw(const Renderer &renderer) const override;

private:
    std::vector<std::shared_ptr<sf::Texture>> m_textures;
    uint32_t m_frames_count = 0;
};

}   // namespace graphics

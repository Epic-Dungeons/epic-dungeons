#pragma once
#include "engine/game_object.h"

namespace graphics {
class Renderer;

class Drawable : public engine::GameObject {
public:
    friend class Renderer;

protected:
    virtual void draw(const Renderer &renderer) const = 0;
};
}   // namespace graphics
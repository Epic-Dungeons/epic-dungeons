#pragma once

namespace graphics {
class Renderer;

class Drawable {
public:
    friend class Renderer;

protected:
    virtual void draw(const Renderer &renderer) const = 0;
};
}   // namespace graphics
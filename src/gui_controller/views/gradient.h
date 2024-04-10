#pragma once
#include "gui_controller/view.h"

namespace gui {
namespace views {

class Gradient : public View {
public:
    Gradient() : View() {
        m_gradient = graphics::Sprite::load("res/sprites/map/gradient.png");
        m_gradient->setPosition({-(int32_t) (cfg::WINDOW_WIDTH / 2), 0})
            .setRotation(-90)
            .setOrigin(graphics::Sprite::Origin::TOP_RIGHT)
            .toSize(cfg::WINDOW_HEIGHT, cfg::WINDOW_WIDTH * 1.5);
    }

    static const std::shared_ptr<Gradient> &getView() {
        static std::shared_ptr<Gradient> view = std::make_shared<Gradient>();
        return view;
    }

protected:
    void draw(const graphics::Renderer &renderer) const override {
        renderer.draw(m_gradient);
    }

private:
    graphics::SpritePtr m_gradient;
};
}   // namespace views
}   // namespace gui

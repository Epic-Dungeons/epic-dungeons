#pragma once

#include "graphics/graphics.h"
#include "gui_controller/controller.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "static_data/game_config.h"

namespace gui {
class Settings : public State {
public:
    void enter(Controller *controller) {
        render(controller->m_renderer.get());
        k_manager.reset();
    }

    void update(Controller *controller) {
        k_manager.update();
        if (k_manager.isClicked(keyboard::KEY_F)) {
            cfg::FPS_COUNTER ^= true;
        }

        if (keyboard::isPressed(keyboard::KEY_ESCAPE)) {
            controller->changeState(GUIState::kMainMenu);
        }
    }

    void render(graphics::Renderer *renderer) {
        renderer->clear();
        renderer->draw(graphics::Text("Settings")
                           .setFontSize(20)
                           .setOrigin(graphics::Text::Origin::TOP_CENTER)
                           .setPosition({cfg::WINDOW_WIDTH / 2, 10}));

        static graphics::Sprite background =
            graphics::Sprite("background/main_menu/background.png").toSizeX(cfg::WINDOW_WIDTH);
        renderer->draw(background);

        int center = cfg::WINDOW_WIDTH * 14 / 36;

        // Current resolution
        static graphics::Text text =
            graphics::Text("").setFontSize(50).setFont("story").setOrigin(graphics::Text::Origin::CENTER);

        renderer->draw(text.setString("Current resolution: " + std::to_string(cfg::WINDOW_WIDTH) + "x"
                                      + std::to_string(cfg::WINDOW_HEIGHT))
                           .setPosition({center, 50}));
        // Frame rate

        renderer->draw(text.setString("Frame rate: " + std::to_string(cfg::FRAMERATE)).setPosition({center, 100}));
        // Controls
        renderer->draw(text.setString("Controls").setPosition({center, 150}));
        renderer->draw(
            text.setString("Move up: " + keyboard::keyToString(cfg::CONTROLS_MOVE_UP)).setPosition({center, 200}));
        renderer->draw(
            text.setString("Move down: " + keyboard::keyToString(cfg::CONTROLS_MOVE_DOWN)).setPosition({center, 250}));
        renderer->draw(
            text.setString("Move left: " + keyboard::keyToString(cfg::CONTROLS_MOVE_LEFT)).setPosition({center, 300}));
        renderer->draw(text.setString("Move right: " + keyboard::keyToString(cfg::CONTROLS_MOVE_RIGHT))
                           .setPosition({center, 350}));
        renderer->draw(
            text.setString("Action: " + keyboard::keyToString(cfg::CONTROLS_ACTION)).setPosition({center, 400}));
        renderer->draw(text.setString("Secondary action: " + keyboard::keyToString(cfg::CONTROLS_SECONDARY))
                           .setPosition({center, 450}));

        // Back
        renderer->draw(text.setString("Press ESC to go back").setPosition({center, 500}));
        renderer->draw(text.setString("Press F to toggle FPS counter").setPosition({center, 550}));
        renderer->display();
    }

private:
    KeyboardManager k_manager;
};
}   // namespace gui

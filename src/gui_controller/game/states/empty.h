#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/game/game_machine.h"
#include "keyboard/keyboard.h"
#include "static_data/game_config.h"
#include <memory>

namespace gui {
namespace game {
class Empty : public GameState {
    virtual void enter(GameMachine *gm) {
        is_pressed = true;
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
        r->clear();
        if (gm->m_engine.lock()->getDungeon()->getCurrentCell().lock()->isVisited()) {
            r->draw(graphics::Text("You are in an empty room").setPosition({50, 50}));
        } else {
            r->draw(graphics::Text("You are first time in this empty room").setPosition({50, 50}));
        }
        r->draw(graphics::Text("Press Enter to continue").setPosition({50, 100}));
        r->display();
    }

    virtual void update(GameMachine *gm) {
        if (!is_pressed && keyboard::isPressed(keyboard::KEY_ENTER)) {
            gm->changeState(GUIGameState::kPostEvent);
        }
        is_pressed = keyboard::isPressed(keyboard::KEY_ENTER);
    }

    bool is_pressed;
};
}   // namespace game
}   // namespace gui

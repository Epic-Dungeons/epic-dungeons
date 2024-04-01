#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/game/game_machine.h"
#include "keyboard/keyboard.h"
#include "static_data/game_config.h"
#include <memory>

namespace gui {
namespace game {
class GameOver : public GameState {
    virtual void enter(GameMachine *gm) {
        auto renderer = gm->m_renderer.lock();
        auto engine = gm->m_engine.lock();
        renderer->draw(graphics::Text("Game Over")
                           .setFontSize(100)
                           .setOrigin(graphics::Text::Origin::CENTER)
                           .setPosition({cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT / 2}));
        renderer->display();
    }
};
}   // namespace game
}   // namespace gui

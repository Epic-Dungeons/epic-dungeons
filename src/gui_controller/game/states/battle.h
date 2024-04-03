#pragma once
#include "dungeon/dungeon.h"
#include "engine/entities.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <cmath>
#include <memory>

namespace gui {
namespace game {
class Battle : public GameState {
    void freeze(GameMachine *gm) override {}

    void unfreeze(GameMachine *gm) override {}

    void enter(GameMachine *gm) override {}
};

}   // namespace game
}   // namespace gui
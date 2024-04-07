#pragma once
#include "dungeon/dungeon.h"
#include "engine/entities.h"
#include "engine/items/consumables/bandage.h"
#include "engine/items/consumables/poison.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "gui_controller/views/item_info.h"
#include "gui_controller/views/skill.h"
#include "gui_controller/views/skill_info.h"
#include "gui_controller/views/skill_selection.h"
#include "gui_controller/views/storage.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <cmath>
#include <memory>

namespace gui {
namespace game {
class Recruit : public GameState {
    std::shared_ptr<engine::entities::Hero> getHero(GameMachine *gm) {
        static std::map<dungeon::Position, std::shared_ptr<engine::entities::Hero>> heroes;
        auto d = gm->m_engine.lock()->getDungeon();
        auto pos = d->getCurrentCell().lock()->getPosition();
        if (heroes.find(pos) == heroes.end()) {
            auto hero = gm->m_engine.lock()->createNewTeammate();
            heroes[pos] = hero;
        }
        return heroes[pos];
    }

    void enter(GameMachine *gm) override {
        auto hero = getHero(gm);
        if (!hero || gm->m_engine.lock()->isRecruited(hero)) {
            gm->changeState(GUIGameState::kCellMovement);
            return;
        }
        logging::debug("Recruit hero: {}", hero->getId());
    }

    void update(GameMachine *gm) override {}
};

}   // namespace game
}   // namespace gui
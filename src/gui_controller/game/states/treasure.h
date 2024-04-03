#pragma once
#include "dungeon/dungeon.h"
#include "engine/entities.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/chest.h"
#include "gui_controller/views/hero.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <cmath>
#include <memory>

namespace gui {
namespace game {

class Treasure : public GameState {
    void freeze(GameMachine *gm) override {}

    void unfreeze(GameMachine *gm) override {
        km.reset();
    }

    void enter(GameMachine *gm) override {
        v_chest.bind(getCurrentChest(gm));
        km.reset();
    }

    std::shared_ptr<engine::Chest> getCurrentChest(GameMachine *gm) {
        auto cell_pos = gm->m_engine.lock()->getDungeon()->getCurrentCell().lock()->getPosition();
        if (m_chests.find(cell_pos) == m_chests.end())
            m_chests[cell_pos] = engine::createChest();
        return m_chests[cell_pos];
    }

    void update(GameMachine *gm) override {
        km.update();
        if (km.isClicked(keyboard::KEY_F)) {
            if (v_chest.isOpen())
                v_chest.close();
            else
                v_chest.open();
        }

        if (v_chest.isOpen()) {
            if (km.isClicked(keyboard::KEY_UP))
                v_chest.previousItem();
            if (km.isClicked(keyboard::KEY_DOWN))
                v_chest.nextItem();
        }

        render(gm->m_renderer.lock(), gm->m_engine.lock());
    }

    void exit(GameMachine *gm) override {}

    void render(const std::shared_ptr<graphics::Renderer> &r, const std::shared_ptr<engine::Engine> &engine) {
        r->clear();
        utils::cellView(r, engine->getDungeon());
        auto party = engine->getParty();
        uint8_t position = 3;
        for (auto &hero : party->getMembers())
            utils::drawEntity(r, hero, position--, false);
        v_chest.setPosition(utils::getEntityPosition(5));
        r->draw(v_chest);
        Vector2d center = {cfg::WINDOW_WIDTH * 4 / 5, cfg::WINDOW_HEIGHT / 2};
        if (!v_chest.isOpen())
            utils::drawMap(r, engine->getDungeon(), center, cfg::CELL_SIZE);
        r->display();
    }

private:
    KeyboardManager km;
    views::Chest v_chest;
    std::map<std::pair<int, int>, std::shared_ptr<engine::Chest>> m_chests;
};

}   // namespace game
}   // namespace gui
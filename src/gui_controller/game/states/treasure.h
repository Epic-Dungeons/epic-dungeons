#pragma once
#include "dungeon/dungeon.h"
#include "engine/entities.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/chest.h"
#include "gui_controller/views/hero.h"
#include "gui_controller/views/map.h"
#include "gui_controller/views/storage.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <cmath>
#include <memory>

namespace gui {
namespace game {

class Treasure : public GameState {
public:
    Treasure() : GameState() {
        v_map = views::Map::getView();
    }

    void freeze(GameMachine *gm) override {}

    void unfreeze(GameMachine *gm) override {
        km.reset();
    }

    void enter(GameMachine *gm) override {
        v_chest.bind(getCurrentChest(gm));
        views::Map::getView()->bind(gm->m_engine.lock()->getDungeon());
        km.reset();
        v_map->bind(gm->m_engine.lock()->getDungeon());
        v_storage.bind(v_chest.getStorage());
        v_storage.setName("\tChest\t->");
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
        if (km.isClicked(keyboard::KEY_TAB)) {
            gm->changeState(GUIGameState::kPartyMenu);
            return;
        }

        if (v_chest.isOpen()) {
            if (km.isClicked(keyboard::Hotkey::MOVE_UP)) {
                v_storage.previousItem();
            }

            if (km.isClicked(keyboard::Hotkey::MOVE_DOWN)) {
                v_storage.nextItem();
            }
            if (km.isClicked(keyboard::Hotkey::MOVE_RIGHT)) {
                v_storage.bind(gm->m_engine.lock()->getParty()->getInventory());
                v_storage.setName("<-\tInventory");
            }
            if (km.isClicked(keyboard::Hotkey::MOVE_LEFT)) {
                v_storage.bind(v_chest.getStorage());
                v_storage.setName("\tChest\t->");
            }
            if (km.isClicked(keyboard::KEY_ENTER) || km.isClicked(keyboard::KEY_SPACE)) {
                if (v_storage.getStorage() == v_chest.getStorage()) {
                    logging::info("Moving item from chest to inventory");
                    v_storage.takeOneSelectedItem(gm->m_engine.lock()->getParty()->getInventory());
                } else {
                    logging::info("Moving item from inventory to chest");
                    v_storage.takeOneSelectedItem(v_chest.getStorage());
                }
            }
        } else {
            if (v_map->isInRoom()) {
                if (km.isClicked(keyboard::Hotkey::MOVE_UP)) {
                    v_map->setTargetRoom(dungeon::Direction::UP);
                }
                if (km.isClicked(keyboard::Hotkey::MOVE_DOWN)) {
                    v_map->setTargetRoom(dungeon::Direction::DOWN);
                }
                if (km.isClicked(keyboard::Hotkey::MOVE_LEFT)) {
                    v_map->setTargetRoom(dungeon::Direction::LEFT);
                }
                if (km.isClicked(keyboard::Hotkey::MOVE_RIGHT)) {
                    v_map->setTargetRoom(dungeon::Direction::RIGHT);
                }
                if (km.isClicked(keyboard::KEY_ENTER) || km.isClicked(keyboard::KEY_SPACE)) {
                    v_map->goForward();
                    gm->changeState(GUIGameState::kMoveTransition);
                    return;
                }
            } else {
                if (km.isClicked(keyboard::Hotkey::MOVE_RIGHT)) {
                    v_map->goForward();
                    gm->changeState(GUIGameState::kMoveTransition);
                    return;
                }
                if (km.isClicked(keyboard::Hotkey::MOVE_LEFT)) {
                    v_map->goBackward();
                    gm->changeState(GUIGameState::kMoveTransition);
                    return;
                }
            }
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
        r->draw(views::Gradient::getView());
        if (!v_chest.isOpen())
            r->draw(v_map->setPosition(center).setDrawVingette(true));
        else
            r->draw(v_storage.setPosition({cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 18})
                        .setState(views::Storage::State::kSelection));
        r->display();
    }

private:
    KeyboardManager km;
    std::shared_ptr<views::Map> v_map;
    views::Chest v_chest;
    views::Storage v_storage;
    std::map<std::pair<int, int>, std::shared_ptr<engine::Chest>> m_chests;
};

}   // namespace game
}   // namespace gui
#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/timed_count.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/dialog.h"
#include "gui_controller/views/gradient.h"
#include "gui_controller/views/map.h"
#include "logging/logger.h"
#include "sound_manager/sound_manager.h"
#include "static_data/game_config.h"
#include <cmath>
#include <map>
#include <memory>

namespace gui {
namespace game {
class CellMovement : public GameState {
public:
    CellMovement() : GameState() {
        auto sigm = [](float x) {
            return 1 / (1 + std::exp(-10 * (x - 0.5)));
        };
        m_prev_anim.init(255, 0, 500, sigm);
        v_map = views::Map::getView();
    }

    virtual void enter(GameMachine *gm) override {
        m_keyboard_manager.reset();
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
        is_in_room = current->isRoom();
        v_map->bind(d);

        if (is_in_room)
            m_prev_anim.start();
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
    }

    virtual void update(GameMachine *gm) override {
        m_keyboard_manager.update();
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        m_prev_anim.update(gm->getDeltaTime());
        if (!m_prev_anim.isEnded()) {
            render(r, gm->m_engine.lock(), m_prev_anim.get());
            return;
        }

        if (m_keyboard_manager.isClicked(keyboard::KEY_F)) {
            gm->m_engine.lock()->getDungeon()->getCurrentCell().lock()->setType(dungeon::CellType::TREASURE);
            gm->changeState(GUIGameState::kTreasure);
            return;
        }

        if (m_keyboard_manager.isClicked(keyboard::KEY_B)) {
            gm->m_engine.lock()->getDungeon()->getCurrentCell().lock()->setType(dungeon::CellType::FIGHT);
            gm->changeState(GUIGameState::kBattle);
            return;
        }

        if (m_keyboard_manager.isClicked(keyboard::KEY_G)) {
            views::Dialog::create()->setTitle("Game Over").setMessage("You have lost!").show();
            return;
        }
        if (m_keyboard_manager.isClicked(keyboard::KEY_TAB)) {
            gm->changeState(GUIGameState::kPartyMenu);
            return;
        }

        bool clicked_up = m_keyboard_manager.isClicked(keyboard::Hotkey::MOVE_UP);
        bool clicked_down = m_keyboard_manager.isClicked(keyboard::Hotkey::MOVE_DOWN);
        bool clicked_right = m_keyboard_manager.isClicked(keyboard::MOVE_RIGHT);
        bool clicked_left = m_keyboard_manager.isClicked(keyboard::Hotkey::MOVE_LEFT);
        bool pressed_right =
            m_keyboard_manager.isPressed(keyboard::KEY_RIGHT) || m_keyboard_manager.isPressed(keyboard::KEY_D);
        bool pressed_left =
            m_keyboard_manager.isPressed(keyboard::KEY_LEFT) || m_keyboard_manager.isPressed(keyboard::KEY_A);
        bool clicked_enter =
            m_keyboard_manager.isClicked(keyboard::KEY_ENTER) || m_keyboard_manager.isClicked(keyboard::KEY_SPACE);
        bool is_clicked = clicked_up || clicked_down || clicked_right || clicked_left || clicked_enter;

        if (!is_in_room) {
            if (pressed_right) {
                v_map->goForward();
                gm->changeState(GUIGameState::kMoveTransition);
                return;
            } else if (pressed_left) {
                v_map->goBackward();
                gm->changeState(GUIGameState::kMoveTransition);
                return;
            }
            render(r, gm->m_engine.lock());
            return;
        }

        if (clicked_up) {
            v_map->setTargetRoom(dungeon::Direction::UP);
        } else if (clicked_right) {
            v_map->setTargetRoom(dungeon::Direction::RIGHT);
        } else if (clicked_down) {
            v_map->setTargetRoom(dungeon::Direction::DOWN);
        } else if (clicked_left) {
            v_map->setTargetRoom(dungeon::Direction::LEFT);
        } else if (clicked_enter) {
            v_map->goForward();
            gm->changeState(GUIGameState::kMoveTransition);
            return;
        }
        render(r, gm->m_engine.lock());
    }

    void render(std::shared_ptr<graphics::Renderer> r, std::shared_ptr<engine::Engine> e,
                float animation_progress = 0.0f) {
        std::shared_ptr<dungeon::Dungeon> d = e->getDungeon();
        std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
        std::shared_ptr<dungeon::Cell> next_cell = d->getNextCell().lock();
        std::shared_ptr<engine::entities::Party> party = e->getParty();
        r->clear();
        utils::cellView(r, d);
        for (size_t i = 0; i < party->getMembersCount(); i++) {
            utils::drawEntity(r, party->getMember(i), 3 - i);
        }
        uint8_t alpha = std::round(m_prev_anim.get());
        r->draw(graphics::Rectangle(0, 0, cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, {0, 0, 0, alpha}));
        Vector2d center = {cfg::WINDOW_WIDTH * 4 / 5, cfg::WINDOW_HEIGHT / 2};
        r->draw(views::Gradient::getView());
        r->draw(v_map->setPosition(center).setAnimationDuration(500));
        r->display();
    }

    void exit(GameMachine *gm) override {
        if (is_in_room) {
            m_prev_anim.start();
        }
    }

private:
    TimedCount m_prev_anim;
    std::shared_ptr<views::Map> v_map;

    KeyboardManager m_keyboard_manager;

    bool is_in_room = true;

    int r_selected = 0;
};
}   // namespace game
}   // namespace gui

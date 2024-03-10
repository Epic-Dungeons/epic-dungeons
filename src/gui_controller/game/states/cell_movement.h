#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/timed_count.h"
#include "gui_controller/utils.h"
#include "static_data/game_config.h"
#include <cmath>
#include <map>
#include <memory>

namespace gui {
namespace game {
class CellMovement : public GameState {
public:
    CellMovement() {
        m_gradient = std::make_shared<graphics::Sprite>("utils/gradient.png");
        m_gradient->setRotation(-90);
        m_gradient->toSize(cfg::WINDOW_HEIGHT, cfg::WINDOW_WIDTH * 1.5);

        auto sigm = [](float x) {
            return 1 / (1 + std::exp(-10 * (x - 0.5)));
        };
        m_prev_anim.init(255, 0, 500, sigm);
    }

    virtual void enter(GameMachine *gm) {
        m_keyboard_manager.reset();
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
        is_in_room = current->isRoom();

        if (is_in_room) {
            m_prev_anim.start();
            neighbours = d->getRoomNeighbours(std::dynamic_pointer_cast<dungeon::Room>(current));
            while (neighbours[r_selected].expired()) {
                r_selected = (r_selected + 1) % 4;
            }
            d->setTargetRoom(neighbours[r_selected].lock());
        }
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
    }

    virtual void update(GameMachine *gm) {
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        m_prev_anim.update(gm->getDeltaTime());
        if (!m_prev_anim.isEnded()) {
            render(r, d, m_prev_anim.get());
            return;
        }

        m_keyboard_manager.update();
        bool clicked_up =
            m_keyboard_manager.isClicked(keyboard::KEY_UP) || m_keyboard_manager.isClicked(keyboard::KEY_W);
        bool clicked_down =
            m_keyboard_manager.isClicked(keyboard::KEY_DOWN) || m_keyboard_manager.isClicked(keyboard::KEY_S);
        bool clicked_right =
            m_keyboard_manager.isClicked(keyboard::KEY_RIGHT) || m_keyboard_manager.isClicked(keyboard::KEY_D);
        bool clicked_left =
            m_keyboard_manager.isClicked(keyboard::KEY_LEFT) || m_keyboard_manager.isClicked(keyboard::KEY_A);
        bool pressed_right =
            m_keyboard_manager.isPressed(keyboard::KEY_RIGHT) || m_keyboard_manager.isPressed(keyboard::KEY_D);
        bool pressed_left =
            m_keyboard_manager.isPressed(keyboard::KEY_LEFT) || m_keyboard_manager.isPressed(keyboard::KEY_A);
        bool clicked_enter =
            m_keyboard_manager.isClicked(keyboard::KEY_ENTER) || m_keyboard_manager.isClicked(keyboard::KEY_SPACE);
        bool is_clicked = clicked_up || clicked_down || clicked_right || clicked_left || clicked_enter;

        if (!is_in_room) {
            if (pressed_right) {
                d->setNextCell(d->getNextOnPath().lock());
                gm->changeState(GUIGameState::kMoveTransition);
            } else if (pressed_left) {
                d->setNextCell(d->getPrevOnPath().lock());
                gm->changeState(GUIGameState::kMoveTransition);
            }
            return;
        }

        if (clicked_up && !neighbours[0].expired()) {
            r_selected = 0;
        } else if (clicked_right && !neighbours[1].expired()) {
            r_selected = 1;
        } else if (clicked_down && !neighbours[2].expired()) {
            r_selected = 2;
        } else if (clicked_left && !neighbours[3].expired()) {
            r_selected = 3;
        } else if (clicked_enter) {
            d->setNextCell(d->getNextOnPath().lock());
            gm->changeState(GUIGameState::kMoveTransition);
        }
        if (is_clicked) {
            d->setTargetRoom(neighbours[r_selected].lock());
            render(r, d);
        }
    }

    void render(std::shared_ptr<graphics::Renderer> r, std::shared_ptr<dungeon::Dungeon> d,
                float animation_progress = 0.0f) {
        std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
        std::shared_ptr<dungeon::Cell> next_cell = d->getNextCell().lock();
        r->clear();
        utils::cellView(r, d);
        uint8_t alpha = std::round(m_prev_anim.get());
        r->drawRec({0, 0, cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, {0, 0, 0, alpha}});
        r->draw(*m_gradient, -(cfg::WINDOW_WIDTH / 2), cfg::WINDOW_HEIGHT);
        Vector2d center = {cfg::WINDOW_WIDTH * 4 / 5, cfg::WINDOW_HEIGHT / 2};
        utils::drawMap(r, d, center, cfg::CELL_SIZE);
        r->display();
    }

    void exit(GameMachine *gm) {
        if (is_in_room) {
            m_prev_anim.start();
        }
    }

private:
    TimedCount m_prev_anim;

    KeyboardManager m_keyboard_manager;

    bool is_in_room = true;

    int r_selected = 0;
    std::vector<std::weak_ptr<dungeon::Room>> neighbours;
    std::shared_ptr<graphics::Sprite> m_gradient;
};
}   // namespace game
}   // namespace gui
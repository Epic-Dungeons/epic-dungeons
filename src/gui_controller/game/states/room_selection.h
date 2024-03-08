#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/game/game_machine.h"
#include "keyboard/keyboard.h"
#include "static_data/game_config.h"
#include <memory>

namespace gui {
namespace game {
class RoomSelection : public GameState {
    virtual void enter(GameMachine *gm) {
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        neighbours = d->getRoomNeighbours(std::dynamic_pointer_cast<dungeon::Room>(d->getCurrentCell().lock()));
        graphics::Renderer *r = gm->m_renderer.lock().get();
        render(r, d);
    }

    virtual void update(GameMachine *gm) {
        graphics::Renderer *r = gm->m_renderer.lock().get();
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        bool pressed_right = keyboard::isPressed(keyboard::KEY_RIGHT) || keyboard::isPressed(keyboard::KEY_D);
        bool pressed_left = keyboard::isPressed(keyboard::KEY_LEFT) || keyboard::isPressed(keyboard::KEY_LEFT);
        bool pressed_enter = keyboard::isPressed(keyboard::KEY_ENTER);

        if (!(pressed_right || pressed_left))
            is_key_pressed = false;

        if (is_key_pressed)
            return;

        if (pressed_right) {
            r_selected = (r_selected + 1) % neighbours.size();
            is_key_pressed = true;
            render(r, d, neighbours[r_selected].lock());
        } else if (pressed_left) {
            r_selected = (r_selected - 1 + neighbours.size()) % neighbours.size();
            is_key_pressed = true;
            render(r, d, neighbours[r_selected].lock());
        } else if (pressed_enter) {
            // next state
        }
    }

    void render(graphics::Renderer *r, std::shared_ptr<dungeon::Dungeon> d, std::shared_ptr<dungeon::Cell> selected_room = nullptr) {
        r->clear();
        r->drawText(50, 50, "Select a room");
        std::vector<std::shared_ptr<dungeon::Cell>> cells = d->getCells();
        for (auto cell : cells) {
            float x = static_cast<float>(cell->getPosition().first) * cfg::CELL_SIZE;
            float y = static_cast<float>(cell->getPosition().second) * cfg::CELL_SIZE;
            float w = cfg::CELL_SIZE;
            graphics::Color color = "#edaf1c";
            graphics::Color stroke_color = "#000000";
            if (cell->isRoom()) {
                x -= cfg::CELL_SIZE;
                y -= cfg::CELL_SIZE;
                w = cfg::CELL_SIZE * 3;
                color = "#cc2e0e";
            }
            if (cell == selected_room)
                stroke_color = "#ffffff";
            if (cell == d->getCurrentCell().lock())
                stroke_color = "#140ecc";
            r->drawRec({x, y, w, w, color, -1, stroke_color});
        }
        r->display();
    }

    void exit(GameMachine *gm) {
        gm->m_engine.lock().get()->getDungeon()->setTargetRoom(neighbours[r_selected]);
    }

private:
    bool is_key_pressed = false;
    int r_selected = 0;
    std::vector<std::weak_ptr<dungeon::Room>> neighbours;
};
}   // namespace game
}   // namespace gui
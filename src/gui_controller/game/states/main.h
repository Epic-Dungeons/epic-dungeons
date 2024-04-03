#pragma once
#include "graphics/graphics.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/timed_count.h"
#include "keyboard/keyboard.h"
#include <cmath>

namespace gui {
namespace game {
class Main : public GameState {
    bool is_ending = false;
    bool is_pressed = false;
    TimedCount m_anim;

    void enter(GameMachine *gm) override {
        is_ending = false;
        auto sigm = [](float x) {
            return 1 / (1 + std::exp(-10 * (x - 0.5)));
        };
        m_anim.init(255, 0, 1000, sigm);
        m_anim.start();

        is_pressed = keyboard::isPressed(keyboard::KEY_ENTER);
        graphics::Renderer *r = gm->m_renderer.lock().get();

        r->clear();
        render(r);
        r->display();
    }

    void update(GameMachine *gm) override {
        gm->changeState(GUIGameState::kEvent);
        return;
        m_anim.update(gm->getDeltaTime());
        if (!m_anim.isEnded()) {
            render(gm->m_renderer.lock().get());
            // return;
        }

        if (is_ending) {
            gm->changeState(GUIGameState::kEvent);
            return;
        }

        if (!is_pressed && keyboard::isPressed(keyboard::KEY_ENTER)) {
            m_anim.init(0, 255, 500, [](float x) {
                return 1 / (1 + std::exp(-10 * (x - 0.5)));
            });
            m_anim.start();
            is_ending = true;
        }
        is_pressed = keyboard::isPressed(keyboard::KEY_ENTER);
    }

    void render(graphics::Renderer *r) {
        u_int8_t alpha = std::round(m_anim.get());
        r->draw(graphics::Text("A long time ago, far, far away...")
                    .setFont("story")
                    .setFontSize(50)
                    .setPosition({100, 100}));
        r->draw(graphics::Text("There was a heroes, who was destined to save the world.")
                    .setFont("story")
                    .setFontSize(50)
                    .setPosition({100, 150}));
        r->draw(graphics::Text("But first, he had to find the way to the dungeon.")
                    .setFont("story")
                    .setFontSize(50)
                    .setPosition({100, 200}));
        r->draw(
            graphics::Text("And so, the adventure begins...").setFont("story").setFontSize(50).setPosition({100, 250}));
        r->draw(graphics::Text("Press ENTER to start").setFont("story").setFontSize(50).setPosition({100, 600}));
        r->draw(graphics::Rectangle(0, 0, cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, {0, 0, 0, alpha}));
        r->display();
    }
};
}   // namespace game
}   // namespace gui

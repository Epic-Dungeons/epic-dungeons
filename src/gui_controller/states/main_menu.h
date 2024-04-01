#pragma once
#include "gui_controller/controller.h"
#include "gui_controller/timed_count.h"
#include "keyboard/keyboard.h"
#include <cmath>

namespace gui {

class MainMenu : public State {
public:
    void enter(Controller *controller) {
        is_ending = false;
        m_selected = 0;
        is_key_pressed = false;

        auto sigm = [](float x) {
            return 1 / (1 + std::exp(-10 * (x - 0.5)));
        };
        m_anim.init(255, 0, 500, sigm);
        m_anim.start();

        render(controller->m_renderer.get());
    }

    void update(Controller *controller) {
        m_anim.update(controller->getDeltaTime());
        if (!m_anim.isEnded()) {
            render(controller->m_renderer.get());
            return;
        }

        if (is_ending) {
            switch (m_selected) {
                case 0:
                    controller->changeState(GUIState::kGame);
                    break;
                case 1:
                    controller->changeState(GUIState::kSettings);
                    break;
                case 2:
                    controller->changeState(GUIState::kExit);
                    break;
            }
            return;
        }

        bool pressed_up = keyboard::isPressed(keyboard::KEY_UP) || keyboard::isPressed(keyboard::KEY_W);
        bool pressed_down = keyboard::isPressed(keyboard::KEY_DOWN) || keyboard::isPressed(keyboard::KEY_S);
        bool pressed_enter = keyboard::isPressed(keyboard::KEY_ENTER);

        if (!(pressed_up || pressed_down || pressed_enter))
            is_key_pressed = false;

        if (is_key_pressed)
            return;

        if (pressed_up) {
            m_selected = (m_selected - 1 + m_options.size()) % m_options.size();
            is_key_pressed = true;
            render(controller->m_renderer.get());

        } else if (pressed_down) {
            m_selected = (m_selected + 1) % m_options.size();
            is_key_pressed = true;
            render(controller->m_renderer.get());
        } else if (pressed_enter) {
            m_anim.init(0, 255, 500, [](float x) {
                return 1 / (1 + std::exp(-10 * (x - 0.5)));
            });
            m_anim.start();
            is_ending = true;
        }
    };

    void exit(Controller *controller) {}

    void render(graphics::Renderer *renderer) {
        renderer->clear();

        uint8_t alpha = std::round(m_anim.get());
        static graphics::Sprite background = graphics::Sprite("background/main_menu/background.png")
                                                 .toSizeX(cfg::WINDOW_WIDTH)
                                                 .setPosition({0, 0});   // NOLINT
        static graphics::Text title = graphics::Text("Epic Dungeon")
                                          .setFontSize(50)
                                          .setFont("medieval")
                                          .setOrigin(graphics::Text::Origin::CENTER)
                                          .setPosition({cfg::WINDOW_WIDTH / 2, 100});

        renderer->draw(background);
        renderer->draw(title);
        for (int i = 0; i < m_options.size(); i++) {
            graphics::Text option = graphics::Text(m_options[i])
                                        .setPosition({cfg::WINDOW_WIDTH / 2, 300 + i * 50})
                                        .setFont("story")
                                        .setFontSize(40)
                                        .setOrigin(graphics::Text::Origin::CENTER);
            renderer->draw(option);
            if (i == m_selected) {
                renderer->draw(graphics::Text("*")
                                   .setFont("story")
                                   .setPosition({cfg::WINDOW_WIDTH * 16 / 36 - 20, 315 + i * 50})
                                   .setFontSize(50));
            }
        }
        renderer->draw(graphics::Rectangle(0, 0, cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, {0, 0, 0, alpha}));
        renderer->display();
    }

private:
    bool is_ending = false;
    TimedCount m_anim;

    int m_selected = 0;
    std::vector<std::string> m_options = {"New Game", "Settings", "Exit"};
    bool is_key_pressed = false;
};

}   // namespace gui

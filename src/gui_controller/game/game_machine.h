#pragma once
#include "gui_controller/controller.h"

namespace gui {
namespace game {
class GameMachine;
class GameState;

enum class GUIGameState {
    kMain,
};

class GameState {
public:
    GameState() = default;

    virtual void enter(GameMachine *gm) {}

    virtual void update(GameMachine *gm) {}

    virtual void exit(GameMachine *gm) {}
};

class GameMachine {
public:
    GameMachine();

    void enter(Controller *controller) {
        if (m_current_state) {
            m_current_state->enter(this);
        }
    }

    void update(Controller *controller) {
        if (m_next_state) {
            m_current_state = m_next_state;
            m_next_state = nullptr;
            m_current_state->enter(this);
        }
        if (m_current_state) {
            m_current_state->update(this);
        }
        if (m_next_state) {
            if (m_current_state) {
                m_current_state->exit(this);
            }
        }
    }

    template<typename T>
    void addState(const GUIGameState &state) {
        m_states[state] = std::make_shared<T>();
    }

    void changeState(const GUIGameState &state) {
        m_next_state = m_states[state];
    }

    void bindEngine(std::weak_ptr<engine::Engine> &engine) {
        m_engine = engine;
    }

    void bindRenderer(std::weak_ptr<renderer::SFMLRenderer> renderer) {
        m_renderer = renderer;
    }

    void exit(Controller *controller) {
        if (m_current_state) {
            m_current_state->exit(this);
        }
    }

    std::weak_ptr<engine::Engine> m_engine;
    std::weak_ptr<renderer::SFMLRenderer> m_renderer;

protected:
    std::shared_ptr<GameState> m_current_state, m_next_state;
    std::map<GUIGameState, std::shared_ptr<GameState>> m_states;
};

}   // namespace game
}   // namespace gui
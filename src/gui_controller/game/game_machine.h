#pragma once
#include "gui_controller/controller.h"

namespace gui {
namespace game {
class GameMachine;
class GameState;

enum class GUIGameState {
    kMain,
    kCellMovement,
    kMoveTransition,
    kEvent,
    kBattle,
    kEmpty,
    kPostEvent,
    kPartyMenu,
    kGameOver,

    // DEMO
    kDialogueDemo
};

class GameState {
public:
    GameState() = default;

    virtual void enter(GameMachine *gm) {}

    virtual void update(GameMachine *gm) {}

    // freeze on pause
    virtual void freeze(GameMachine *gm) {
        exit(gm);
    }

    virtual void exit(GameMachine *gm) {}

    // unfreeze on resume
    virtual void unfreeze(GameMachine *gm) {
        enter(gm);
    }
};

class GameMachine {
public:
    GameMachine();

    void unfreeze(Controller *controller) {
        if (m_current_state) {
            m_current_state->unfreeze(this);
        }
    }

    void update(Controller *controller) {
        m_delta_time = controller->getDeltaTime();
        if (m_next_state) {
            m_current_state = m_next_state;
            m_previous_state_id = m_current_state_id;
            m_current_state_id = m_next_state_id;
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
        m_next_state_id = state;
    }

    void bindEngine(std::weak_ptr<engine::Engine> &engine) {
        m_engine = engine;
    }

    void bindRenderer(std::weak_ptr<graphics::Renderer> renderer) {
        m_renderer = renderer;
    }

    void freeze(Controller *controller) {
        if (m_current_state) {
            m_current_state->freeze(this);
        }
    }

    uint64_t getDeltaTime() {
        return m_delta_time;
    }

    std::weak_ptr<engine::Engine> m_engine;
    std::weak_ptr<graphics::Renderer> m_renderer;
    GUIGameState m_current_state_id;
    GUIGameState m_previous_state_id;
    GUIGameState m_next_state_id;

protected:
    std::shared_ptr<GameState> m_current_state, m_next_state;
    std::map<GUIGameState, std::shared_ptr<GameState>> m_states;
    uint64_t m_delta_time;
};

}   // namespace game
}   // namespace gui

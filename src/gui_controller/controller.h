#pragma once
#include "engine/engine.h"
#include "graphics/graphics.h"

namespace gui {
class Controller;
class State;
enum class GUIState {
    kMainMenu,
    kSettings,
    kGame,
    kFight,
    kRoomSelection,
    kDirectionChoice,
    kGameOver,
    kExit,

    // development GUIstates:
    kDialogueDemo
};

class State {
    friend class Controller;

public:
    State() = default;

    virtual void enter(Controller *controller) {}

    virtual void update(Controller *controller) {}

    virtual void exit(Controller *controller) {}

    virtual void onEngineBind(std::weak_ptr<engine::Engine> engine) {}

    virtual void onRendererBind(std::weak_ptr<graphics::Renderer> renderer) {}
};

class Controller {
    friend class State;

public:
    Controller();

    void bindEngine(std::shared_ptr<engine::Engine> engine) {
        m_engine = engine;
        for (auto &state : m_states) {
            state.second->onEngineBind(engine);
        }
    }

    void bindRenderer(std::shared_ptr<graphics::Renderer> renderer) {
        m_renderer = renderer;
        for (auto &state : m_states) {
            state.second->onRendererBind(renderer);
        }
    }

    template<typename T>
    void addState(const GUIState &state) {
        m_states[state] = std::make_shared<T>();
    }

    void changeState(const GUIState &state) {
        m_next_state = m_states[state];
    }

    virtual void update() {
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

    uint64_t getDeltaTime() {
        return delta_time;
    }

    void setDeltaTime(uint64_t dt) {
        delta_time = dt;
    }

    std::shared_ptr<graphics::Renderer> m_renderer;
    std::shared_ptr<engine::Engine> m_engine;

protected:
    uint64_t delta_time;
    std::shared_ptr<State> m_current_state;
    std::shared_ptr<State> m_next_state;
    std::map<GUIState, std::shared_ptr<State>> m_states;
};

}   // namespace gui

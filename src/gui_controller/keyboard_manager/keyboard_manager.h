#pragma once
#include "keyboard/keyboard.h"
#include <vector>

namespace gui {
class KeyboardManager {
public:
    KeyboardManager() : m_cur_state(49, false), m_prev_state(49, false) {}

    void update() {
        for (int i = 0; i < m_cur_state.size(); i++) {
            m_prev_state[i] = m_cur_state[i];
            m_cur_state[i] = keyboard::isPressed(keyboard::Key(i));
        }
    }

    void reset() {
        for (int i = 0; i < m_cur_state.size(); i++) {
            m_prev_state[i] = keyboard::isPressed(keyboard::Key(i));
            m_cur_state[i] = keyboard::isPressed(keyboard::Key(i));
        }
    }

    bool isPressed(const keyboard::Key &key) const {
        return m_cur_state[key];
    }

    bool isPressed(const keyboard::Hotkey &hotkey) const {
        switch (hotkey) {
            case keyboard::Hotkey::MOVE_UP:
                return isPressed(keyboard::Key::KEY_UP) || isPressed(keyboard::Key::KEY_W);
            case keyboard::Hotkey::MOVE_DOWN:
                return isPressed(keyboard::Key::KEY_DOWN) || isPressed(keyboard::Key::KEY_S);
            case keyboard::Hotkey::MOVE_LEFT:
                return isPressed(keyboard::Key::KEY_LEFT) || isPressed(keyboard::Key::KEY_A);
            case keyboard::Hotkey::MOVE_RIGHT:
                return isPressed(keyboard::Key::KEY_RIGHT) || isPressed(keyboard::Key::KEY_D);
            default:
                return false;
        }
    }

    bool isClicked(const keyboard::Key &key) const {
        return m_cur_state[key] && !m_prev_state[key];
    }

    bool isClicked(const keyboard::Hotkey &hotkey) const {
        switch (hotkey) {
            case keyboard::Hotkey::MOVE_UP:
                return isClicked(keyboard::Key::KEY_UP) || isClicked(keyboard::Key::KEY_W);
            case keyboard::Hotkey::MOVE_DOWN:
                return isClicked(keyboard::Key::KEY_DOWN) || isClicked(keyboard::Key::KEY_S);
            case keyboard::Hotkey::MOVE_LEFT:
                return isClicked(keyboard::Key::KEY_LEFT) || isClicked(keyboard::Key::KEY_A);
            case keyboard::Hotkey::MOVE_RIGHT:
                return isClicked(keyboard::Key::KEY_RIGHT) || isClicked(keyboard::Key::KEY_D);
            default:
                return false;
        }
    }

private:
    std::vector<uint8_t> m_cur_state;
    std::vector<uint8_t> m_prev_state;
};
}   // namespace gui

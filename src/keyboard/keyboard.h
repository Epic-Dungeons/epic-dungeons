#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>

namespace keyboard {
enum Key {
    UNKNOWN = -1,
    KEY_1 = 0,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_ENTER,
    KEY_ESCAPE,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_SHIFT,
    KEY_CTRL,
    KEY_ALT,
    KEY_SUPER,
};

enum Hotkey {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
};

class KeyboardController {
public:
    static KeyboardController &getInstance() {
        static KeyboardController instance;
        return instance;
    }

    void setKeyState(const Key &key, bool state);
    const bool isPressed(const Key &key) const;

    // Prevent copying and assignment
    KeyboardController(const KeyboardController &) = delete;
    KeyboardController &operator=(const KeyboardController &) = delete;

private:
    KeyboardController() : key_states(50, false) {}

    std::vector<bool> key_states;
};

const Key sfmlToKey(const sf::Keyboard::Key &key);
const std::string keyToString(const Key &key);

static void setKeyState(const Key &key, const bool &state) {
    KeyboardController::getInstance().setKeyState(key, state);
}

static void setKeyState(const sf::Keyboard::Key &key, const bool &state) {
    setKeyState(sfmlToKey(key), state);
}

static const bool isPressed(const Key &key) {
    return KeyboardController::getInstance().isPressed(key);
}

}   // namespace keyboard

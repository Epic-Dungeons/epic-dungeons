#pragma once
#include "keyboard/keyboard.h"
#include <spdlog/spdlog.h>
#include <string>

namespace cfg {
const int FRAMERATE = 30;
inline bool FPS_COUNTER = false;
const unsigned WINDOW_WIDTH = 1280;
const unsigned WINDOW_HEIGHT = 800;
const unsigned CELL_SIZE = 20;
const std::string WINDOW_NAME = "*EPIC DUNGEONS*";
const std::string FONTS_PATH = "res/fonts/";
const std::string SPRITES_PATH = "res/sprites/";
const uint32_t ANIMATION_FRAME_DURATION = 33;
const std::string SOUNDS_PATH = "res/sounds/";

const keyboard::Key CONTROLS_MOVE_RIGHT = keyboard::Key::KEY_RIGHT;
const keyboard::Key CONTROLS_MOVE_DOWN = keyboard::Key::KEY_DOWN;
const keyboard::Key CONTROLS_MOVE_LEFT = keyboard::Key::KEY_LEFT;
const keyboard::Key CONTROLS_MOVE_UP = keyboard::Key::KEY_UP;
const keyboard::Key CONTROLS_ACTION = keyboard::Key::KEY_Z;
const keyboard::Key CONTROLS_CLOSE = keyboard::Key::KEY_X;
const keyboard::Key CONTROLS_SECONDARY = keyboard::Key::KEY_C;

const uint32_t DIALOGUE_FONT_SIZE = 24;
const uint32_t DIALOGUE_FONT_SPEED = 30;
const std::string FONT_PATH = "courier";
const std::string DEFAULT_FONT = "courier";

// !! Later put fonts into epic-dungeons/res/fonts !!
#ifdef _WIN32
const std::string FONT_PATH = "C:/Windows/Fonts/arial.ttf";
#elif __linux__
const std::string FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";
#elif __APPLE__
//const std::string FONT_PATH = "/Library/Fonts/Arial Unicode.ttf";
#endif

#ifdef DEBUG
inline spdlog::level::level_enum LOG_LEVEL = spdlog::level::info;
#elif RELEASE
inline spdlog::level::level_enum LOG_LEVEL = spdlog::level::info;
#else
inline spdlog::level::level_enum LOG_LEVEL = spdlog::level::warn;
#endif

}   // namespace cfg

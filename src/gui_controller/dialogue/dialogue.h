#pragma once
#include "graphics/graphics.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/timed_count.h"
#include "static_data/game_config.h"
#include <functional>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

namespace gui {
namespace dl {
const uint32_t DIALOGUE_WINDOW_WIDTH = cfg::WINDOW_WIDTH;
const uint32_t DIALOGUE_WINDOW_HEIGHT = cfg::WINDOW_HEIGHT / 4.5;
const uint32_t WINDOW_MARGIN = 16;
const uint32_t WINDOW_PADDING = 8;
const uint32_t PORTRAIT_SIZE = DIALOGUE_WINDOW_HEIGHT - WINDOW_MARGIN * 2;
const uint32_t LINE_HEIGHT = 36;   // pixels
const uint32_t LINE_LENGTH = 47;   // characters
const uint32_t CHAR_WIDTH = 18;    // pixels

std::string preprocessString(const std::string &str);

namespace script {
class ScriptNode {
public:
    virtual ~ScriptNode() {};
};

class QuoteNode : public ScriptNode {
public:
    QuoteNode(std::string content, std::string sprite, ScriptNode *next,
              std::function<void(gui::game::GameMachine *gm)> meta_action) :
        content(std::move(content)),
        sprite(std::move(sprite)), next(next), meta_action(std::move(meta_action)) {}

    std::string content;
    std::string sprite;
    ScriptNode *next;
    std::function<void(gui::game::GameMachine *gm)> meta_action;
};

class ChoiceNode : public ScriptNode {
public:
    ChoiceNode(std::vector<ScriptNode *> next_pool, std::vector<std::string> string_pool,
               std::function<void(gui::game::GameMachine *gm)> meta_action) :
        next_pool(std::move(next_pool)),
        string_pool(std::move(string_pool)), meta_action(std::move(meta_action)) {}

    std::vector<ScriptNode *> next_pool;
    std::vector<std::string> string_pool;
    std::function<void(gui::game::GameMachine *gm)> meta_action;
};
}   // namespace script

class DialogueWindow {   // to draw single dialogue window
public:
    DialogueWindow();
    DialogueWindow(const std::string &content, const std::string &sprite);
    void changeQuote(const std::string &new_content, const std::string &new_sprite);
    void drawQuote(std::shared_ptr<graphics::Renderer> renderer);
    void drawChoice(std::shared_ptr<graphics::Renderer> renderer, std::vector<std::string> lines,
                    uint32_t active_choice, std::vector<script::ScriptNode *> next_steps) const;
    void update(uint32_t current_character);
    void forceFinish();
    bool isFinished();

private:
    std::vector<std::string> m_content;
    std::string m_sprite;
    graphics::Color m_text_color = 0xffffff;
    uint32_t m_current_index;
    uint32_t m_content_len;
    uint32_t m_current_line;
    uint32_t m_font_size;
    mutable int m_delay = 0;
    bool m_is_finished;
    // TODO: REFACTOR LATER
    std::shared_ptr<graphics::Sprite> m_demo_sprite;
};

class DialogueManager {   // to manage dialogue branches, meta-actions etc
public:
    DialogueManager();
    void setEntryPoint(script::ScriptNode *entry_point, gui::game::GameMachine *gm);
    void nextQuote(gui::game::GameMachine *gm);
    void skip();
    void update(uint64_t delta_time);
    void draw(std::shared_ptr<graphics::Renderer> renderer);
    bool isActive() const;
    bool isChoice() const;
    void handleActionKeyPressed(
        gui::game::GameMachine *gm);   // nextQuote() or skip() call depending of text animation status
    void nextChoice();
    void prevChoice();
    void choose(gui::game::GameMachine *gm);
    void handleKeyboard(gui::KeyboardManager &keyboard_manager, gui::game::GameMachine *gm);

private:
    bool m_is_active;
    bool m_is_dialogue;
    script::ScriptNode *m_current_quote;
    DialogueWindow m_dialogue_window;
    gui::TimedCount m_character_anim;
    uint32_t m_active_choice;
    std::vector<script::ScriptNode *> m_next_steps;
    std::vector<std::string> m_choice_lines;
};
}   // namespace dl
}   // namespace gui

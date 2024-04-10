#include "dialogue.h"
#include "graphics/graphics.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/timed_count.h"
#include "static_data/game_config.h"
#include <cmath>
#include <functional>
#include <string>

using namespace gui::dl;

namespace METADATA {
std::string SHORTPAUSE = "$p$p$p$p$p$p$p$p$p$p$p";
std::string LONGPAUSE = "$p$p$p$p$p$p$p$p$p$p$p$p$p$p$p$p$p$p";
const int COLOR_TAG_LENGTH = 7;
const int CHAR_SIZE_TAG_LENGTH = 6;
};   // namespace METADATA

std::string gui::dl::preprocessString(const std::string &str) {
    std::string result = "";
    size_t last_index = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i - 1] == '$')
            continue;   // preprocess ignore character.
        if (str[i] == ',') {
            result += (str.substr(last_index, i - last_index + 1) + METADATA::SHORTPAUSE);
            last_index = i + 1;
        }
        if (str[i] == '.') {
            result += (str.substr(last_index, i - last_index + 1) + METADATA::LONGPAUSE);
            last_index = i + 1;
        }
    }
    result += str.substr(last_index);
    return result;
}

std::vector<std::string> preprocessVector(const std::vector<std::string> &vec) {
    std::vector<std::string> result;
    std::transform(vec.begin(), vec.end(), std::back_inserter(result), [](std::string item) {
        return preprocessString(item);
    });
    return result;
}

std::vector<std::string> splitByLines(const std::string &str) {
    std::vector<std::string> result;
    std::string current_line = "";
    bool is_metadata = false;
    uint32_t current_characters = 0;
    std::string current_word = "";
    uint32_t current_word_len;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '$' || str[i] == '[')
            is_metadata = true;
        if (str[i - 1] == '$' || str[i] == ']') {
            is_metadata = false;
            current_word.push_back(str[i]);
            continue;
        }
        if (!is_metadata) {
            if (str[i] == ' ') {
                if (current_word_len + current_characters < LINE_LENGTH) {
                    current_line += current_word + ' ';
                    current_characters += current_word_len;
                    current_word = "";
                    current_word_len = 0;
                } else {
                    result.push_back(current_line);
                    current_line = current_word + ' ';
                    current_characters = current_word_len + 1;
                    current_word_len = 0;
                    current_word = "";
                }
                continue;
            }
            if (str[i] == '\n') {
                current_line += current_word;
                result.push_back(current_line);
                current_characters = 0;
                current_word = "";
                current_word_len = 0;
                current_line = "";
                continue;
            }
        }
        current_word.push_back(str[i]);
        if (!is_metadata)
            current_word_len++;
    }
    current_line += current_word;
    result.push_back(current_line);
    return result;
}

DialogueWindow::DialogueWindow(const std::string &content, const std::string &sprite) :
    m_content(splitByLines(preprocessString(content))), m_sprite(sprite), m_content_len(content.length()),
    m_current_index(0), m_current_line(0), m_font_size(cfg::DIALOGUE_FONT_SIZE), m_is_finished(false),
    m_demo_sprite(std::make_shared<graphics::Sprite>("res/sprites/test1.png")) {}

DialogueWindow::DialogueWindow() :
    m_content(std::vector<std::string>()), m_sprite(""), m_content_len(0), m_current_index(0), m_current_line(0),
    m_font_size(cfg::DIALOGUE_FONT_SIZE), m_is_finished(false),
    m_demo_sprite(std::make_shared<graphics::Sprite>("res/sprites/test1.png")) {}

void DialogueWindow::changeQuote(const std::string &new_content, const std::string &new_sprite) {
    std::string preprocessed_str = preprocessString(new_content);
    m_content = (splitByLines(preprocessed_str));
    m_sprite = new_sprite;
    m_content_len = preprocessed_str.length();
    m_current_line = 0;
    m_current_index = 0;
    m_is_finished = false;
};

void DialogueWindow::forceFinish() {
    m_current_index = m_content_len;
    m_is_finished = true;
}

inline bool DialogueWindow::isFinished() {
    return m_is_finished;
}

void DialogueWindow::drawQuote(std::shared_ptr<graphics::Renderer> renderer) {
    // CONTAINER
    renderer->draw(
        graphics::Rectangle({WINDOW_MARGIN, cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN},
                            {DIALOGUE_WINDOW_WIDTH - WINDOW_MARGIN * 2, DIALOGUE_WINDOW_HEIGHT - WINDOW_MARGIN * 2})
            .setColor(0x000000)
            .setOutlineThickness(5)
            .setOutlineColor(0xffffff));

    // PORTRAIT (REFACTOR LATER)
    m_demo_sprite->toSize(PORTRAIT_SIZE, PORTRAIT_SIZE)
        .setColor(0xffffff)
        .setPosition({WINDOW_MARGIN, cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN});
    renderer->draw(m_demo_sprite);

    // TEXT
    uint32_t current_len = 0;
    uint32_t char_pos = 0;
    bool display = true;

    for (int i = 0; i < m_content.size(); i++) {
        std::string line = m_content[i];
        char_pos = 0;

        for (int j = 0; j < line.length(); j++) {   // I will explain this later
            // parse metadata
            display = true;
            if (line[j] == '$' || line[j - 1] == '$') {
                display = false;
            } else if (line[j] == '[' && line[j + 1] == '/') {
                if (line.substr(j, METADATA::COLOR_TAG_LENGTH) == "[/color") {
                    m_text_color = 0xffffff;
                    j += METADATA::COLOR_TAG_LENGTH;
                }
                if (line.substr(j, METADATA::CHAR_SIZE_TAG_LENGTH) == "[/size") {
                    m_font_size = 24;
                    j += METADATA::CHAR_SIZE_TAG_LENGTH;
                }
                continue;
            } else if (line[j] == '[') {   // handle metadata
                if (line.substr(j, METADATA::COLOR_TAG_LENGTH) == "[color=") {
                    j += METADATA::COLOR_TAG_LENGTH;
                    m_text_color = graphics::Color::fromHex(line.substr(j, METADATA::COLOR_TAG_LENGTH));
                    j += METADATA::COLOR_TAG_LENGTH + 1;
                }
                if (line.substr(j, METADATA::CHAR_SIZE_TAG_LENGTH) == "[size=") {
                    j += METADATA::CHAR_SIZE_TAG_LENGTH;
                    m_font_size = std::stoi(line.substr(j, line.find(']') - j));
                    j += std::to_string(m_font_size).length() + 1;
                }
                // handle actions
                // TODO handle actions
            } else {
                char_pos++;
            }

            // draw character
            if (display && current_len < m_current_index)
                renderer->draw(graphics::Text(std::string(&line[j], &line[j + 1]))
                                   .setFillColor(m_text_color)
                                   .setFontSize(m_font_size)
                                   .setPosition({WINDOW_MARGIN + WINDOW_PADDING + char_pos * CHAR_WIDTH,
                                                 cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN
                                                     + WINDOW_PADDING + LINE_HEIGHT * i}));
            if (m_current_index < m_content_len)
                current_len++;
            else {
                m_is_finished = true;
            }
        }
    }
}

void DialogueWindow::drawChoice(std::shared_ptr<graphics::Renderer> renderer, std::vector<std::string> lines,
                                uint32_t active_choice, std::vector<script::ScriptNode *> next_steps) const {
    // CONTAINER
    renderer->draw(
        graphics::Rectangle({WINDOW_MARGIN, cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN},
                            {DIALOGUE_WINDOW_WIDTH - WINDOW_MARGIN * 2, DIALOGUE_WINDOW_HEIGHT - WINDOW_MARGIN * 2})
            .setColor(0x000000)
            .setOutlineThickness(5)
            .setOutlineColor(0xffffff));

    uint32_t rec_padding = 4;
    // CONTENT
    for (int i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        if (i == active_choice) {
            renderer->draw(
                graphics::Rectangle({WINDOW_MARGIN + WINDOW_PADDING - rec_padding,
                                     cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN + WINDOW_PADDING
                                         + LINE_HEIGHT * i - rec_padding},
                                    {DIALOGUE_WINDOW_WIDTH - WINDOW_MARGIN * 2 - rec_padding * 2 - WINDOW_PADDING,
                                     rec_padding * 2 + LINE_HEIGHT})
                    .setColor(0xffffff));

            renderer->draw(graphics::Text(line).setFillColor(0x000000).setPosition(
                {WINDOW_MARGIN + WINDOW_PADDING,
                 10 + cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN + WINDOW_PADDING + LINE_HEIGHT * i}));
        } else {
            renderer->draw(graphics::Text(line).setPosition(
                {WINDOW_MARGIN + WINDOW_PADDING,
                 10 + cfg::WINDOW_HEIGHT - DIALOGUE_WINDOW_HEIGHT + WINDOW_MARGIN + WINDOW_PADDING + LINE_HEIGHT * i}));
        }
    }
}

void DialogueWindow::update(uint32_t current_character) {
    if (!m_is_finished)
        m_current_index = current_character;
    else
        m_current_index = m_content_len;
}

DialogueManager::DialogueManager() :
    m_current_quote(nullptr), m_dialogue_window(DialogueWindow()), m_character_anim(gui::TimedCount()),
    m_is_active(false) {
    m_character_anim.init(0, 0, 0);
    m_character_anim.start();
}

void DialogueManager::nextQuote(gui::game::GameMachine *gm) {
    script::QuoteNode *quote = dynamic_cast<script::QuoteNode *>(m_current_quote);
    if (quote != nullptr && quote->next == nullptr) {
        m_is_active = false;
        return;
    }
    if (!m_dialogue_window.isFinished()) {
        m_dialogue_window.forceFinish();
        return;
    }
    setEntryPoint(quote->next, gm);
}

inline void DialogueManager::skip() {
    m_dialogue_window.forceFinish();
    m_is_active = false;
}

void DialogueManager::handleActionKeyPressed(gui::game::GameMachine *gm) {
    if (m_is_active) {
        skip();
    } else {
        nextQuote(gm);
    }
}

void DialogueManager::update(uint64_t delta_time) {
    m_character_anim.update(delta_time);
    m_dialogue_window.update(std::round(m_character_anim.get()));
}

void DialogueManager::draw(std::shared_ptr<graphics::Renderer> renderer) {
    if (m_is_active && m_current_quote != nullptr) {
        if (m_is_dialogue) {
            m_dialogue_window.drawChoice(renderer, m_choice_lines, m_active_choice, m_next_steps);
        } else {
            m_dialogue_window.drawQuote(renderer);
        }
    }
}

void DialogueManager::setEntryPoint(script::ScriptNode *entry_point, gui::game::GameMachine *gm) {
    m_current_quote = entry_point;
    script::QuoteNode *quote = dynamic_cast<script::QuoteNode *>(entry_point);
    if (quote != nullptr) {   // if script node
        std::invoke(quote->meta_action, gm);
        m_dialogue_window.changeQuote(quote->content, quote->sprite);
        size_t str_len = dl::preprocessString(quote->content).length();
        m_character_anim.init(0, str_len, str_len * cfg::DIALOGUE_FONT_SPEED);
        m_character_anim.start();
        m_is_active = 1;
        m_is_dialogue = 0;
    } else {   // if choice node
        script::ChoiceNode *choice = dynamic_cast<script::ChoiceNode *>(entry_point);
        std::invoke(choice->meta_action, gm);
        m_next_steps = choice->next_pool;
        m_choice_lines = choice->string_pool;
        m_is_active = 1;
        m_is_dialogue = 1;
        m_active_choice = 0;
    }
}

bool DialogueManager::isActive() const {
    return !m_is_active;
}

bool DialogueManager::isChoice() const {
    return m_is_dialogue;
}

void DialogueManager::nextChoice() {
    if (!m_is_dialogue)
        return;
    m_active_choice = (m_active_choice + 1) % m_choice_lines.size();
}

void DialogueManager::prevChoice() {
    if (!m_is_dialogue)
        return;
    m_active_choice -= 1;
    if (m_active_choice + 1 == 0)
        m_active_choice = m_choice_lines.size() - 1;
}

void DialogueManager::choose(gui::game::GameMachine *gm) {
    if (!m_is_dialogue)
        return;
    setEntryPoint(m_next_steps[m_active_choice], gm);
}

void DialogueManager::handleKeyboard(gui::KeyboardManager &keyboard_manager, gui::game::GameMachine *gm) {
    this->update(gm->getDeltaTime());
    if (keyboard_manager.isClicked(cfg::CONTROLS_ACTION)) {
        if (this->isChoice()) {
            this->choose(gm);
        } else {
            this->nextQuote(gm);
        }
    }
    if (keyboard_manager.isClicked(cfg::CONTROLS_MOVE_DOWN))
        this->nextChoice();
    if (keyboard_manager.isClicked(cfg::CONTROLS_MOVE_UP))
        this->prevChoice();
}

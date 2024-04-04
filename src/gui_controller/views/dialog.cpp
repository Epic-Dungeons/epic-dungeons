#include "dialog.h"
#include "graphics/graphics.h"
#include "gui_controller/dialog_manager.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "static_data/game_config.h"
#include <memory>

namespace gui {
namespace views {
Dialog::Dialog() : m_title("Demo"), m_message("Demo dialog") {}

std::shared_ptr<Dialog> Dialog::create() {
    return std::make_shared<Dialog>();
}

Dialog &Dialog::setTitle(const std::string &title) {
    m_title = title;
    return *this;
}

Dialog &Dialog::setMessage(const std::string &message) {
    m_message = message;
    return *this;
}

void Dialog::update() {
    km.update();
    if (km.isClicked(keyboard::KEY_ENTER)) {
        close();
    }
}

void Dialog::show() {
    auto dialog = std::dynamic_pointer_cast<Dialog>(shared_from_this());
    DialogManager::showDialog(dialog);
}

void Dialog::close() {
    auto dialog = std::dynamic_pointer_cast<Dialog>(shared_from_this());
    DialogManager::closeDialog(dialog);
}

void Dialog::draw(const graphics::Renderer &renderer) const {
    renderer.draw(graphics::Rectangle({cfg::WINDOW_WIDTH / 4, cfg::WINDOW_HEIGHT / 4},
                                      {cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT / 2})
                      .setColor(graphics::Color(0, 0, 0, 200))
                      .setOutlineColor(0xffffff)
                      .setOutlineThickness(2));
    renderer.draw(graphics::Text(m_title)
                      .setPosition({cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT / 3})
                      .setFontSize(50)
                      .setOrigin(graphics::Text::Origin::CENTER));
    renderer.draw(graphics::Text(m_message)
                      .setPosition({cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT / 2})
                      .setFontSize(30)
                      .setOrigin(graphics::Text::Origin::CENTER));
    renderer.draw(graphics::Text("Press Enter to close")
                      .setPosition({cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT * 3 / 4 - cfg::WINDOW_HEIGHT / 2 / 8})
                      .setFontSize(20)
                      .setOrigin(graphics::Text::Origin::BOTTOM_CENTER));
}

}   // namespace views
}   // namespace gui

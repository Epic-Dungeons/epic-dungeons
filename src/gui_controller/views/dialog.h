#pragma once

#include "dungeon/dungeon.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/timed_count.h"
#include "gui_controller/view.h"

namespace gui {
namespace views {
class Dialog : public View {
public:
    Dialog();
    static std::shared_ptr<Dialog> create();

    virtual ~Dialog() = default;

    Dialog &setTitle(const std::string &title);
    Dialog &setMessage(const std::string &message);

    virtual void update();

    void show();
    void close();

protected:
    virtual void draw(const graphics::Renderer &renderer) const override;
    KeyboardManager km;
    std::string m_title;
    std::string m_message;
};

}   // namespace views
}   // namespace gui
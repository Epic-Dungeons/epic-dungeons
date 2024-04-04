#pragma once
#include "graphics/graphics.h"
#include "keyboard_manager/keyboard_manager.h"
#include <memory>

namespace gui {

namespace views {
class Dialog;
}   // namespace views

class DialogManager {
public:
    DialogManager() = delete;
    static void showDialog(const std::shared_ptr<views::Dialog> &dialog);
    static void closeDialog(const std::shared_ptr<views::Dialog> &dialog);
    static void closeAllDialogs();

    static bool update();
    static void render(const graphics::Renderer &renderer);

private:
    // Prevent instantiation
    ~DialogManager() = default;
    DialogManager(const DialogManager &) = delete;
    DialogManager &operator=(const DialogManager &) = delete;

    KeyboardManager km;
    static std::vector<std::shared_ptr<views::Dialog>> m_dialogs;
};

}   // namespace gui
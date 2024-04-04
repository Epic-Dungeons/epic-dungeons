#include "dialog_manager.h"
#include "logging/logger.h"
#include "views/dialog.h"

namespace gui {

std::vector<std::shared_ptr<views::Dialog>> DialogManager::m_dialogs = {};

void DialogManager::showDialog(const std::shared_ptr<views::Dialog> &dialog) {
    m_dialogs.push_back(dialog);
}

void DialogManager::closeDialog(const std::shared_ptr<views::Dialog> &dialog) {
    m_dialogs.erase(std::find(m_dialogs.begin(), m_dialogs.end(), dialog));   // Remove dialog from vector
}

void DialogManager::closeAllDialogs() {
    m_dialogs.clear();
}

bool DialogManager::update() {   // true if there are dialogs
    bool result = false;
    for (auto &dialog : m_dialogs) {
        dialog->update();
        result = true;
    }
    return result;
}

void DialogManager::render(const graphics::Renderer &renderer) {
    for (const auto &dialog : m_dialogs) {
        renderer.draw(*dialog);
    }
    renderer.display();
}

}   // namespace gui
#pragma once
#include "engine/storage.h"
#include "gui_controller/view.h"

namespace gui {
namespace views {

class Storage : public View {
public:
    Storage() : View() {}

    enum class State {
        kSelection,
        kNone,
    };

    Storage &bind(const std::shared_ptr<engine::items::Storage> &storage);
    Storage &setMaxVisibleItems(const size_t &max_visible_items);
    Storage &setPosition(const Vector2d &position);
    Storage &setState(const State &state);
    Storage &setSelectedItem(const size_t &selected_item);
    Storage &setName(const std::string &name);
    Storage &updateSelected();

    Storage &nextItem();
    Storage &previousItem();

    std::shared_ptr<engine::items::Item> getSelectedItem() const;

protected:
    void draw(const graphics::Renderer &renderer) const override;

private:
    std::weak_ptr<engine::items::Storage> m_storage;
    std::string m_name = "Storage";
    size_t m_selected_item = 0;
    size_t m_max_visible_items = 0;
    mutable size_t m_items_offset = 0;
    Vector2d m_position = {0, 0};
    State m_state = State::kNone;
};

}   // namespace views
}   // namespace gui
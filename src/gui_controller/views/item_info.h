#pragma once
#include "engine/entity.h"
#include "engine/item.h"
#include "gui_controller/view.h"
#include <memory>

namespace gui {
namespace views {

class ItemInfo : public View {
public:
    ItemInfo() : View() {}

    ItemInfo &bind(const std::shared_ptr<engine::items::Item> &item);

    /// @brief Bind entity to the view to add info for consumable items
    ItemInfo &bindEntity(const std::shared_ptr<engine::entities::Entity> &entity);
    ItemInfo &setPosition(const Vector2d &position);

protected:
    void draw(const graphics::Renderer &renderer) const override;

private:
    std::weak_ptr<engine::items::Item> m_item;
    std::weak_ptr<engine::entities::Entity> m_entity;
    Vector2d m_position = {0, 0};
};

}   // namespace views
}   // namespace gui
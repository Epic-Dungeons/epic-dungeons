#pragma once

#include "engine/chest.h"
#include "graphics/graphics.h"
#include "gui_controller/view.h"
#include "storage.h"
#include <memory>

namespace gui {
namespace views {
class Chest : public View {
public:
    Chest();

    Chest &bind(const std::shared_ptr<engine::Chest> &chest);
    void open();
    void close();
    Chest &setPosition(const Vector2d &position);
    const bool isOpen() const;
    const std::shared_ptr<engine::Chest> getChest() const;
    const std::shared_ptr<engine::items::Storage> getStorage() const;
    const std::shared_ptr<Storage> getStorageView() const;

    Chest &nextItem();
    Chest &previousItem();

protected:
    void draw(const graphics::Renderer &renderer) const override;
    std::weak_ptr<engine::Chest> m_chest;
    std::shared_ptr<Storage> v_storage;
    std::shared_ptr<graphics::Sprite> m_sprite;
    Vector2d m_position = {0, 0};
    bool is_open = false;
};

}   // namespace views
}   // namespace gui
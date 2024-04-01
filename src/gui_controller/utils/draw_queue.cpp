
#include "engine/engine.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "logging/logger.h"

namespace gui {
namespace utils {

void drawQueue(const std::shared_ptr<graphics::Renderer> &renderer,
               const std::vector<std::weak_ptr<views::Entity>> &queue, const uint32_t &selected) {
    const float center = cfg::WINDOW_WIDTH / 2;
    const float cell_size = cfg::WINDOW_WIDTH / 15;
    const float margin_above = cell_size * 0.3;
    const float spacing = cell_size * 0.1;
    const uint8_t queue_size = queue.size();
    const float queue_width = queue_size * (cell_size + spacing) - spacing;
    const float queue_left = center - queue_width / 2;
    const float queue_top = margin_above;
    const float selected_margin = cell_size * 0.2;

    for (uint8_t i = 0; i < queue_size; i++) {
        auto entity = queue[i].lock();
        std::shared_ptr<graphics::Sprite> portrait = entity->getPortrait();
        portrait->toSize(cell_size, cell_size);
        if (entity->getEntity()->isAlive()) {
            portrait->setColor({255, 255, 255, 255});
        } else {
            portrait->setColor({255, 9, 9, 65});
        }
        renderer->draw(portrait.get()->setPosition({queue_left + i * (cell_size + spacing), queue_top}));
    }
    if (selected < queue_size) {
        auto entity = queue[selected].lock();
        std::shared_ptr<graphics::Sprite> portrait = entity->getPortrait();
        portrait->toSize(cell_size + selected_margin, cell_size + selected_margin);
        renderer->draw(portrait.get()->setPosition(
            {queue_left + selected * (cell_size + spacing) - selected_margin / 2, queue_top - selected_margin / 2}));
    }
}

}   // namespace utils
}   // namespace gui
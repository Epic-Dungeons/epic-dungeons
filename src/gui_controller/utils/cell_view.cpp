#include "dungeon/dungeon.h"
#include "engine/engine.h"
#include "gui_controller/utils.h"
#include "logging/logger.h"
#include "static_data/game_config.h"

namespace gui {
namespace utils {

void cellView(const std::shared_ptr<graphics::Renderer> &renderer, const std::shared_ptr<dungeon::Dungeon> &dungeon,
              const float &animation_progress) {
    static bool is_loaded = false;
    static float hall_width;
    static std::vector<graphics::Sprite> m_hall_backgrounds;
    static std::vector<graphics::Sprite> m_room_backgrounds;
    if (!is_loaded) {
        logging::info("Loading cell view sprites");
        m_hall_backgrounds.push_back(graphics::Sprite("background/hall/crypts/0.png"));
        m_hall_backgrounds.push_back(graphics::Sprite("background/hall/crypts/1.png"));
        m_hall_backgrounds.push_back(graphics::Sprite("background/hall/crypts/2.png"));
        for (auto &sprite : m_hall_backgrounds) {
            sprite.toSizeY(cfg::WINDOW_HEIGHT * 5 / 6);
        }
        hall_width = m_hall_backgrounds[0].getSize().x;

        m_room_backgrounds.push_back(graphics::Sprite("background/room/crypts/empty.png"));
        for (auto &sprite : m_room_backgrounds) {
            sprite.toSizeY(cfg::WINDOW_HEIGHT * 5 / 6);
        }

        is_loaded = true;
        logging::info("Cell view sprites loaded");
    }

    std::shared_ptr<graphics::Renderer> r = renderer;
    std::shared_ptr<dungeon::Dungeon> d = dungeon;
    std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
    bool is_in_room = current->isRoom();
    if (is_in_room) {
        // uint8_t alpha = 255 * (1 - animation_progress);
        // m_room_backgrounds[0].setColor({255, 255, 255});
        r->draw(m_room_backgrounds[0]);
        return;
    }

    int direction = 1;
    std::shared_ptr<dungeon::Cell> next_cell = d->getNextCell().lock();
    if (next_cell && next_cell != d->getNextOnPath().lock()) {
        direction = -1;
    }

    uint8_t alpha = 255;
    // if (next_cell->isRoom())
    //     alpha = 255 * (1 - animation_progress);

    int distance_to_target = d->getDistanceToTarget();
    for (int i = 0; i < 5; i++) {
        int idx = (distance_to_target - i + 99) % 3;
        m_hall_backgrounds[idx].setColor({255, 255, 255, alpha});
        r->draw(
            m_hall_backgrounds[idx].setPosition({hall_width * (i - animation_progress * direction) - hall_width, 0}));
    }
}

}   // namespace utils
}   // namespace gui

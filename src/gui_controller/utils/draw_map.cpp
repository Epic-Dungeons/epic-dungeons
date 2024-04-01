#include "dungeon/dungeon.h"
#include "engine/engine.h"
#include "graphics/graphics.h"
#include "gui_controller/utils.h"
#include "logging/logger.h"
#include <map>

namespace gui {
namespace utils {

void drawMap(const std::shared_ptr<graphics::Renderer> &renderer, const std::shared_ptr<dungeon::Dungeon> &dungeon,
             const Vector2d &center, const float &cell_size, const float &animation_progress) {
    // load static textures
    const std::string map_path = "map/";
    static bool is_loaded = false;
    static std::map<dungeon::CellType, graphics::SpritePtr> m_hall_sprites;
    static std::map<dungeon::CellType, graphics::SpritePtr> m_room_sprites;
    static graphics::SpritePtr m_visited_room_sprite;
    static graphics::SpritePtr m_not_visited_room_sprite;
    static graphics::SpritePtr m_not_visited_hall_sprite;
    static graphics::SpritePtr m_selected_room_sprite;
    static graphics::SpritePtr m_current_cell_sprite;
    if (!is_loaded) {
        // clang-format off
        logging::info("Loading map sprites");
        m_hall_sprites = {
            {dungeon::CellType::NOTHING,    graphics::Sprite::load(map_path + "hall/clear.png")},
            {dungeon::CellType::FIGHT,      graphics::Sprite::load(map_path + "hall/battle.png")},
            {dungeon::CellType::TREASURE,   graphics::Sprite::load(map_path + "hall/curio.png")},
            {dungeon::CellType::TRAP,       graphics::Sprite::load(map_path + "hall/trap.png")},
        };
        m_room_sprites = {
            {dungeon::CellType::NOTHING,    graphics::Sprite::load(map_path + "room/empty.png")},
            {dungeon::CellType::FIGHT,      graphics::Sprite::load(map_path + "room/battle.png")},
            {dungeon::CellType::BOSS,       graphics::Sprite::load(map_path + "room/boss.png")},
            {dungeon::CellType::TREASURE,   graphics::Sprite::load(map_path + "room/treasure.png")},
        };
        m_visited_room_sprite =             graphics::Sprite::load(map_path + "room/marker_visited.png");
        m_not_visited_room_sprite =         graphics::Sprite::load(map_path + "room/unknown.png");
        m_not_visited_hall_sprite =         graphics::Sprite::load(map_path + "hall/dark.png");
        m_selected_room_sprite =            graphics::Sprite::load(map_path + "room/marker_moving.png");
        m_current_cell_sprite =             graphics::Sprite::load(map_path + "indicator.png");
        // clang-format on
        is_loaded = true;
        logging::info("Map sprites loaded");
    }
    std::shared_ptr<graphics::Renderer> r = renderer;
    std::shared_ptr<dungeon::Dungeon> d = dungeon;

    std::vector<std::shared_ptr<dungeon::Cell>> cells = d->getCells();
    std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
    std::shared_ptr<dungeon::Room> m_target_room = d->getTargetRoom().lock();
    std::shared_ptr<dungeon::Cell> m_next_cell = d->getNextCell().lock();

    float cx = static_cast<float>(current->getPosition().first) * cell_size;
    float cy = static_cast<float>(current->getPosition().second) * cell_size;
    float dx = 0;
    float dy = 0;
    float nw = (current->isRoom() ? 3 : 1) * cell_size;
    if (m_next_cell) {
        dx = (m_next_cell->getPosition().first - current->getPosition().first);
        dy = (m_next_cell->getPosition().second - current->getPosition().second);
        nw = (m_next_cell->isRoom() ? 3 : 1) * cell_size;
    }
    dx *= cell_size * animation_progress;
    dy *= cell_size * animation_progress;
    float x_offset = cx - center.x + dx;
    float y_offset = cy - center.y + dy;
    cx -= x_offset;
    cy -= y_offset;

    for (auto cell : cells) {
        float x = static_cast<float>(cell->getPosition().first) * cell_size - x_offset;
        float y = static_cast<float>(cell->getPosition().second) * cell_size - y_offset;
        float w = cell_size;
        graphics::SpritePtr base_sprite = nullptr;
        graphics::SpritePtr marker_sprite = nullptr;

        if (cell->isRoom()) {
            w = cell_size * 3;
            base_sprite = m_room_sprites[cell->getType()];
            if (cell->isVisited()) {
                marker_sprite = m_visited_room_sprite;
            } else {
                base_sprite = m_not_visited_room_sprite;
            }
        } else {
            base_sprite = m_hall_sprites[cell->getType()];
            if (!cell->isVisited()) {
                base_sprite = m_not_visited_hall_sprite;
            }
        }
        base_sprite->toSize(w, w);
        x -= w / 2;
        y -= w / 2;
        float alpha_distance = (center.x - x) * (center.x - x) + (center.y - y) * (center.y - y);
        alpha_distance = 1 - alpha_distance / (cell_size * cell_size * 168);
        alpha_distance *= 255;
        alpha_distance = std::max(0, std::min(255, static_cast<int>(alpha_distance)));
        base_sprite->setColor({255, 255, 255, (uint8_t) alpha_distance});
        r->draw(base_sprite->setPosition({x, y}));
        if (marker_sprite) {
            marker_sprite->toSize(w, w);
            marker_sprite->setColor({255, 255, 255, (uint8_t) alpha_distance});
            r->draw(marker_sprite->setPosition({x, y}));
        }
        if (cell == m_target_room) {
            m_selected_room_sprite->toSize(w, w);
            r->draw(m_selected_room_sprite->setPosition({x, y}));
        }
    }
    float x = cx + dx;
    float y = cy + dy;
    float w = cell_size;
    if (current->isRoom()) {
        w = cell_size * 3;
    }
    w = ((float) nw - w) * animation_progress + w;
    m_current_cell_sprite->toSize(w, w);
    r->draw(m_current_cell_sprite->setPosition({x - w / 2, y - w / 2}));
}
}   // namespace utils
}   // namespace gui

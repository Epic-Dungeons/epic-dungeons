#include "map.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <cmath>
#include <map>
#include <memory>

namespace gui {
namespace views {

namespace {
static std::map<std::string, std::shared_ptr<graphics::Animation>> animations;
static std::map<std::string, std::shared_ptr<graphics::Sprite>> sprites;

static std::shared_ptr<graphics::Animation> getAnimation(const std::string &path) {
    if (animations.find(path) == animations.end()) {
        animations[path] = std::make_shared<graphics::Animation>(path);
    }
    return animations[path];
}

static std::shared_ptr<graphics::Sprite> getSprite(const std::string &path) {
    if (sprites.find(path) == sprites.end()) {
        sprites[path] = std::make_shared<graphics::Sprite>(path);
    }
    return sprites[path];
}
}   // namespace

Map::Map() : View() {
    const std::string map_path = "map/";
    // clang-format off
    logging::info("Loading map sprites");
    m_hall_sprites = {
        {dungeon::CellType::NOTHING,    getSprite(map_path + "hall/clear.png")},
        {dungeon::CellType::FIGHT,      getSprite(map_path + "hall/battle.png")},
        {dungeon::CellType::TREASURE,   getSprite(map_path + "hall/curio.png")},
        {dungeon::CellType::TRAP,       getSprite(map_path + "hall/trap.png")},
    };
    m_room_sprites = {
        {dungeon::CellType::NOTHING,    getSprite(map_path + "room/empty.png")},
        {dungeon::CellType::FIGHT,      getSprite(map_path + "room/battle.png")},
        {dungeon::CellType::BOSS,       getSprite(map_path + "room/boss.png")},
        {dungeon::CellType::TREASURE,   getSprite(map_path + "room/treasure.png")},
    };
    m_visited_room_sprite =             getSprite(map_path + "room/marker_visited.png");
    m_not_visited_room_sprite =         getSprite(map_path + "room/unknown.png");
    m_not_visited_hall_sprite =         getSprite(map_path + "hall/dark.png");
    m_selected_room_sprite =            getSprite(map_path + "room/marker_moving.png");
    m_current_cell_sprite =             getSprite(map_path + "indicator.png");
    // clang-format on
    logging::info("Map sprites loaded");
    m_animation.setFunction([](float x) {
        return 1 / (1 + std::exp(-10 * (x - 0.5)));   // sigmoid function
    });
    m_animation.init(0, 1, 500);
}

Map &Map::bind(const std::shared_ptr<dungeon::Dungeon> &dungeon) {
    if (dungeon == nullptr)
        m_dungeon.reset();
    else {
        m_dungeon = dungeon;
        bool is_in_room = dungeon->getCurrentCell().lock()->isRoom();
        if (is_in_room) {
            while (!setTargetRoom(m_target_room))
                m_target_room = static_cast<dungeon::Direction>((static_cast<int>(m_target_room) + 1) % 4);
        }
    }
    return *this;
}

void Map::goForward() {
    auto dungeon = m_dungeon.lock();
    if (!dungeon) {
        return;
    }
    m_prev_cell = dungeon->getCurrentCell();
    dungeon->setNextCell(dungeon->getNextOnPath().lock());
}

void Map::goBackward() {
    auto dungeon = m_dungeon.lock();
    if (!dungeon) {
        return;
    }
    m_prev_cell = dungeon->getCurrentCell();
    dungeon->setNextCell(dungeon->getPrevOnPath().lock());
}

Map &Map::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

Map &Map::setVingetteDistance(const float &distance) {
    m_vingette_distance = distance;
    return *this;
}

Map &Map::setDrawVingette(const bool &draw_vingette) {
    m_draw_vingette = draw_vingette;
    return *this;
}

Map &Map::setCellSize(const float &cell_size) {
    m_cell_size = cell_size;
    return *this;
}

Map &Map::setAnimationDuration(const uint64_t &duration) {
    m_animation.init(0, 1, duration);
    return *this;
}

Map &Map::setState(const State &state) {
    m_state = state;
    return *this;
}

bool Map::setTargetRoom(const dungeon::Direction &direction) {
    auto d = m_dungeon.lock();
    if (!d)
        return false;
    auto current_room = std::dynamic_pointer_cast<dungeon::Room>(d->getCurrentCell().lock());
    auto neighbours = d->getRoomNeighbours(current_room);
    if (neighbours.at(static_cast<int>(direction)).expired())
        return false;
    auto room = std::dynamic_pointer_cast<dungeon::Room>(neighbours.at(static_cast<int>(direction)).lock());
    d->setTargetRoom(room);
    m_target_room = direction;
    return true;
}

std::shared_ptr<dungeon::Dungeon> Map::getDungeon() const {
    return m_dungeon.lock();
}

std::shared_ptr<dungeon::Room> Map::getTargetRoom() const {
    auto dungeon = m_dungeon.lock();
    if (!dungeon) {
        return nullptr;
    }
    return dungeon->getTargetRoom().lock();
}

std::shared_ptr<dungeon::Cell> Map::getCurrentCell() const {
    auto dungeon = m_dungeon.lock();
    if (!dungeon) {
        return nullptr;
    }
    return dungeon->getCurrentCell().lock();
}

void Map::draw(const graphics::Renderer &renderer) const {
    if (m_dungeon.expired())
        return;

    auto r = renderer;
    auto d = m_dungeon.lock();

    bool is_in_room = d->getCurrentCell().lock()->isRoom();
    m_animation.update();

    std::vector<std::shared_ptr<dungeon::Cell>> cells = d->getCells();
    std::shared_ptr<dungeon::Room> m_target_room = d->getTargetRoom().lock();
    std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
    std::shared_ptr<dungeon::Cell> m_next_cell = d->getCurrentCell().lock();
    if (!d->getNextCell().expired())
        m_next_cell = d->getNextCell().lock();

    float cx = static_cast<float>(current->getPosition().first) * m_cell_size;
    float cy = static_cast<float>(current->getPosition().second) * m_cell_size;
    float animation_progress = 1;
    if (!m_animation.isEnded())
        animation_progress = m_animation.get();
    float dx = 0;
    float dy = 0;
    float nw = (current->isRoom() ? 3 : 1) * m_cell_size;
    if (m_next_cell) {
        dx = (m_next_cell->getPosition().first - current->getPosition().first);
        dy = (m_next_cell->getPosition().second - current->getPosition().second);
        nw = (m_next_cell->isRoom() ? 3 : 1) * m_cell_size;
    }
    dx *= m_cell_size * animation_progress;
    dy *= m_cell_size * animation_progress;
    float x_offset = cx - m_position.x + dx;
    float y_offset = cy - m_position.y + dy;
    cx -= x_offset;
    cy -= y_offset;

    for (auto cell : cells) {
        float x = static_cast<float>(cell->getPosition().first) * m_cell_size - x_offset;
        float y = static_cast<float>(cell->getPosition().second) * m_cell_size - y_offset;
        float w = m_cell_size;
        graphics::SpritePtr base_sprite = nullptr;
        graphics::SpritePtr marker_sprite = nullptr;

        if (cell->isRoom()) {
            w = m_cell_size * 3;
            base_sprite = m_room_sprites.find(cell->getType())->second;
            if (cell->isVisited()) {
                marker_sprite = m_visited_room_sprite;
            } else {
                base_sprite = m_not_visited_room_sprite;
            }
        } else {
            base_sprite = m_hall_sprites.find(cell->getType())->second;
            if (!cell->isVisited()) {
                base_sprite = m_not_visited_hall_sprite;
            }
        }
        base_sprite->toSize(w, w);
        x -= w / 2;
        y -= w / 2;
        float alpha_distance = 255;
        if (m_draw_vingette) {
            alpha_distance = (m_position.x - x) * (m_position.x - x) + (m_position.y - y) * (m_position.y - y);
            alpha_distance =
                1 - alpha_distance / (m_cell_size * m_cell_size * m_vingette_distance * m_vingette_distance);
            alpha_distance *= 255;
            alpha_distance = std::max(0, std::min(255, static_cast<int>(alpha_distance)));
        }
        base_sprite->setColor({255, 255, 255, (uint8_t) alpha_distance});
        r.draw(base_sprite->setPosition({x, y}));
        if (marker_sprite) {
            marker_sprite->toSize(w, w);
            marker_sprite->setColor({255, 255, 255, (uint8_t) alpha_distance});
            r.draw(marker_sprite->setPosition({x, y}));
        }
        if (cell == m_target_room) {
            m_selected_room_sprite->toSize(w, w);
            r.draw(m_selected_room_sprite->setPosition({x, y}));
        }
    }
    float x = cx + dx;
    float y = cy + dy;
    float w = m_cell_size;
    if (current->isRoom()) {
        w = m_cell_size * 3;
    }
    w = ((float) nw - w) * animation_progress + w;
    m_current_cell_sprite->toSize(w, w);
    r.draw(m_current_cell_sprite->setPosition({x - w / 2, y - w / 2}));
}

const std::shared_ptr<Map> &Map::getView() {
    static std::shared_ptr<Map> view = std::make_shared<Map>();
    return view;
}

void Map::startAnimation() {
    m_animation.start();
}

bool Map::isAnimationEnded() const {
    return m_animation.isEnded();
}

bool Map::isInRoom() const {
    return m_dungeon.lock()->getCurrentCell().lock()->isRoom();
}

}   // namespace views
}   // namespace gui
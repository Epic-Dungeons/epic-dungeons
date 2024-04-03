#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/timed_count.h"
#include "gui_controller/view.h"

namespace gui {
namespace views {

class Map : public View {
public:
    Map();

    static const std::shared_ptr<Map> &getView();

    enum class State {
        kRoomSelection,
        kNone,
    };

    Map &bind(const std::shared_ptr<dungeon::Dungeon> &dungeon);
    Map &setState(const State &state);
    Map &setPosition(const Vector2d &position);
    Map &setVingetteDistance(const float &distance);
    Map &setDrawVingette(const bool &draw_vingette);
    Map &setCellSize(const float &cell_size);
    Map &setAnimationDuration(const uint64_t &duration);

    bool setTargetRoom(const dungeon::Direction &direction);

    void goForward();
    void goBackward();

    void startAnimation();
    bool isAnimationEnded() const;

    bool isInRoom() const;

    std::shared_ptr<dungeon::Dungeon> getDungeon() const;
    std::shared_ptr<dungeon::Room> getTargetRoom() const;
    std::shared_ptr<dungeon::Cell> getCurrentCell() const;

protected:
    void draw(const graphics::Renderer &renderer) const override;

private:
    std::weak_ptr<dungeon::Dungeon> m_dungeon;
    std::weak_ptr<dungeon::Cell> m_prev_cell;

    std::map<dungeon::CellType, graphics::SpritePtr> m_hall_sprites;
    std::map<dungeon::CellType, graphics::SpritePtr> m_room_sprites;
    graphics::SpritePtr m_visited_room_sprite;
    graphics::SpritePtr m_not_visited_room_sprite;
    graphics::SpritePtr m_not_visited_hall_sprite;
    graphics::SpritePtr m_selected_room_sprite;
    graphics::SpritePtr m_current_cell_sprite;

    State m_state = State::kNone;
    dungeon::Direction m_target_room = dungeon::Direction::UP;
    bool m_draw_vingette = true;
    float m_vingette_distance = 13.0f;
    float m_cell_size = 20;
    Vector2d m_position = {0, 0};

    mutable TimedCount m_animation;
};

}   // namespace views
}   // namespace gui
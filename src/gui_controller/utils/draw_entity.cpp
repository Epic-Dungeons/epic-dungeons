#include "engine/engine.h"
#include "engine/skill.h"
#include "graphics/graphics.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include "vector2d/vector2d.h"

namespace gui {
namespace utils {

const Vector2d &getEntityPosition(const uint8_t &position) {
    static const float space_left = 0.0f;
    static const float space_right = cfg::WINDOW_WIDTH * 0.95f;
    static const float space_width = space_right - space_left;
    static const float entities_bottom = cfg::WINDOW_HEIGHT * 9 / 12;
    static const float mid = (space_left + space_right) / 2;
    static const float team_width = space_width * 4.2 / 9;
    static const float team_spacing = team_width * 0.28;
    static const float team_mid_offset = team_width * 1.7 / 10;

    static std::vector<Vector2d> positions = {
        {mid - team_mid_offset - team_spacing * 3, entities_bottom},
        {mid - team_mid_offset - team_spacing * 2, entities_bottom},
        {mid - team_mid_offset - team_spacing * 1, entities_bottom},
        {mid - team_mid_offset - team_spacing * 0, entities_bottom},
        {mid + team_mid_offset + team_spacing * 0, entities_bottom},
        {mid + team_mid_offset + team_spacing * 1, entities_bottom},
        {mid + team_mid_offset + team_spacing * 2, entities_bottom},
        {mid + team_mid_offset + team_spacing * 3, entities_bottom},
    };
    return positions[position];
};

void drawEntity(const std::shared_ptr<graphics::Renderer> &renderer,
                const std::shared_ptr<engine::entities::Entity> &entity, const uint8_t &position, bool is_selected,
                const float &animation_progress) {
    auto view = views::Entity::getView(entity);
    Vector2d m_position = getEntityPosition(position);
    view->setPosition(m_position);
    if (position >= 4)
        view->setDirection(views::Entity::Direction::kLeft);
    else if (position < 4)
        view->setDirection(views::Entity::Direction::kRight);
    view->setSelection(is_selected ? views::Entity::Selection::kSelected : views::Entity::Selection::kNone);
    renderer->draw(view);
}
}   // namespace utils
}   // namespace gui

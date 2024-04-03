#pragma once
#include "graphics/graphics.h"
#include "vector2d/vector2d.h"
#include <memory>

namespace engine {
class Engine;

namespace skills {
class Skill;
}   // namespace skills

namespace entities {
class Entity;
}   // namespace entities
}   // namespace engine

namespace dungeon {
class Dungeon;
}   // namespace dungeon

namespace gui {
namespace views {
class Entity;
}   // namespace views

namespace utils {
void cellView(const std::shared_ptr<graphics::Renderer> &renderer, const std::shared_ptr<dungeon::Dungeon> &dungeon,
              const float &animation_progress = 0.0f);

void drawEntity(const std::shared_ptr<graphics::Renderer> &renderer,
                const std::shared_ptr<engine::entities::Entity> &entity, const uint8_t &position,
                bool is_selected = false, const float &animation_progress = 0.0f);

void drawGUI(const std::shared_ptr<graphics::Renderer> &renderer, const std::shared_ptr<engine::Engine> &engine);

void drawQueue(const std::shared_ptr<graphics::Renderer> &renderer,
               const std::vector<std::weak_ptr<views::Entity>> &queue, const uint32_t &selected = 10);

void drawSkills(const std::shared_ptr<graphics::Renderer> &renderer,
                const std::vector<std::shared_ptr<engine::skills::Skill>> &queue, const uint32_t &selected = 10);

const Vector2d &getEntityPosition(const uint8_t &position);

}   // namespace utils
}   // namespace gui

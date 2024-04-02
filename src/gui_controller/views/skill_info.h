#pragma once
#include "engine/entity.h"
#include "engine/skill.h"
#include "gui_controller/view.h"
#include <memory>

namespace gui {
namespace views {

class SkillInfo : public View {
public:
    SkillInfo() : View() {}

    SkillInfo &bind(const std::shared_ptr<engine::skills::Skill> &item);
    SkillInfo &bindEntity(const std::shared_ptr<engine::entities::Entity> &entity);
    SkillInfo &setPosition(const Vector2d &position);

protected:
    void draw(const graphics::Renderer &renderer) const override;

private:
    std::weak_ptr<engine::skills::Skill> m_skill;
    std::weak_ptr<engine::entities::Entity> m_entity;
    Vector2d m_position = {0, 0};
};

}   // namespace views
}   // namespace gui
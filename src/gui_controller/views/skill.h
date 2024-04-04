#pragma once
#include "engine/skill.h"
#include "graphics/graphics.h"
#include "gui_controller/view.h"
#include "vector2d/vector2d.h"
#include <memory>
#include <string>

namespace gui {
namespace views {

class Skill : public View {
public:
    Skill(const std::shared_ptr<engine::skills::Skill> &skill);

    enum class Selection {
        kNone,
        kSelected,
        kSelectable,
        kNotSelectable,
    };

    const std::shared_ptr<engine::skills::Skill> getSkill() const {
        return m_skill;
    }

    static const std::shared_ptr<Skill> getView(const std::shared_ptr<engine::skills::Skill> &skill);

    void reset();

    Skill &setPosition(const Vector2d &position);
    Skill &setSelection(const Selection &selection);
    Skill &setSize(const Vector2d &size);

    Selection getSelection() const;

    Vector2d getSize() const;

protected:
    void draw(const graphics::Renderer &renderer) const override;

    std::shared_ptr<engine::skills::Skill> m_skill;
    std::shared_ptr<graphics::Sprite> m_sprite;
    Selection m_selection;
    Vector2d m_position = {0, 0};
};

}   // namespace views
}   // namespace gui
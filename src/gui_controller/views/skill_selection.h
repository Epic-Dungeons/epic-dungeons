#pragma once
#include "gui_controller/view.h"
#include "skill.h"

namespace gui {
namespace views {

class SkillSelection : public View {
public:
    SkillSelection() : View() {};

    enum class State { kNone, kSelection };
    enum class Origin {
        kCenter,
        kTopLeft,
        kTopCenter,
        kBottomRight,
    };

    SkillSelection &setPosition(const Vector2d &position);

    SkillSelection &bind(const std::vector<std::shared_ptr<engine::skills::Skill>> &skills);
    SkillSelection &nextSkill();
    SkillSelection &previousSkill();
    SkillSelection &setOrigin(const Origin &origin);
    SkillSelection &setState(const State &state);
    SkillSelection &selectSkill(size_t index);
    SkillSelection &setTitle(const std::string &title);

    const std::shared_ptr<engine::skills::Skill> getSelectedSkill() const;

protected:
    void draw(const graphics::Renderer &renderer) const override;

private:
    std::string m_title = "Skills";
    std::vector<std::shared_ptr<Skill>> m_skills;
    Vector2d m_position = {0, 0};
    size_t m_selected = 0;
    State m_state = State::kNone;
    Origin m_origin = Origin::kTopLeft;
};

}   // namespace views
}   // namespace gui
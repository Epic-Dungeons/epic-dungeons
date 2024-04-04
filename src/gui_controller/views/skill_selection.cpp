#include "skill_selection.h"

namespace gui {
namespace views {

namespace {
Vector2d getGridPosition(size_t index, size_t columns, double cell_size) {
    size_t row = index / columns;
    size_t column = index % columns;
    return {column * cell_size, row * cell_size};
}

}   // namespace

SkillSelection &SkillSelection::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

SkillSelection &SkillSelection::bind(const std::vector<std::shared_ptr<engine::skills::Skill>> &skills) {
    m_skills.clear();
    for (const auto &skill : skills) {
        m_skills.push_back(Skill::getView(skill));
    }
    m_selected = 0;
    return *this;
}

SkillSelection &SkillSelection::nextSkill() {
    if (m_selected + 1 < m_skills.size()) {
        size_t new_selected = m_selected + 1;
        while (new_selected < m_skills.size()
               && m_skills[new_selected]->getSelection() == Skill::Selection::kNotSelectable) {
            new_selected++;
        }
        if (m_skills[new_selected]->getSelection() != Skill::Selection::kNotSelectable)
            m_selected = new_selected;
        else {
            logging::debug("SkillSelection::nextSkill: No more selectable skills");
        }
    }

    return *this;
}

SkillSelection &SkillSelection::previousSkill() {
    if (m_selected > 0) {
        size_t new_selected = m_selected - 1;
        while (new_selected > 0 && m_skills[new_selected]->getSelection() == Skill::Selection::kNotSelectable) {
            new_selected--;
        }
        if (m_skills[new_selected]->getSelection() != Skill::Selection::kNotSelectable)
            m_selected = new_selected;
        else {
            logging::debug("SkillSelection::previousSkill: No more selectable skills");
        }
    }
    return *this;
}

SkillSelection &SkillSelection::selectSkill(size_t index) {
    if (index < m_skills.size())
        m_selected = index;
    return *this;
}

const std::shared_ptr<engine::skills::Skill> SkillSelection::getSelectedSkill() const {
    if (m_selected < m_skills.size())
        return m_skills[m_selected]->getSkill();
    return nullptr;
}

void SkillSelection::draw(const graphics::Renderer &renderer) const {
    if (m_skills.empty())
        return;
    Vector2d position = m_position;
    const size_t grid_cols = 3;
    const float cell_size = m_skills[0]->getSize().x;
    const float grid_width = (grid_cols - 1) * cell_size * 1.2 + cell_size;
    Vector2d offset = {0, 0};
    if (m_origin == Origin::kCenter)
        offset = {-grid_width / 2, cell_size / 2};
    else if (m_origin == Origin::kTopCenter)
        offset = {-grid_width / 2, 0};
    else if (m_origin == Origin::kBottomRight)
        offset = {-grid_width, -cell_size * 1.2};
    else if (m_origin == Origin::kTopLeft)
        offset = {0, 0};
    position += offset;

    for (size_t i = 0; i < m_skills.size(); i++) {
        m_skills[i]->setPosition(position + getGridPosition(i, grid_cols, cell_size * 1.2) + Vector2d {0, 70});
        if (m_state == State::kSelection && m_skills[i]->getSelection() != Skill::Selection::kNotSelectable)
            m_skills[i]->setSelection(i == m_selected ? Skill::Selection::kSelected : Skill::Selection::kSelectable);
        else if (m_state == State::kNone)
            m_skills[i]->setSelection(Skill::Selection::kNone);
        renderer.draw(m_skills[i]);
    }
    renderer.draw(graphics::Text(m_title)
                      .setPosition(position)
                      .setOrigin(graphics::Text::BOTTOM_LEFT)
                      .setFontSize(50)
                      .setFont("story"));
}

SkillSelection &SkillSelection::setOrigin(const Origin &origin) {
    m_origin = origin;
    return *this;
}

SkillSelection &SkillSelection::setState(const State &state) {
    m_state = state;
    return *this;
}

SkillSelection &SkillSelection::setTitle(const std::string &title) {
    m_title = title;
    return *this;
}

std::shared_ptr<Skill> SkillSelection::getSkill(size_t index) const {
    if (index < m_skills.size())
        return m_skills[index];
    return nullptr;
}

const std::vector<std::shared_ptr<Skill>> &SkillSelection::getSkills() const {
    return m_skills;
}

bool SkillSelection::fixSelection() {
    if (m_selected >= m_skills.size())
        m_selected = 0;
    if (m_skills.empty())
        return false;
    int32_t new_selected = m_selected;
    while (new_selected < m_skills.size()
           && m_skills[new_selected]->getSelection() == Skill::Selection::kNotSelectable) {
        new_selected++;
    }
    if (new_selected < m_skills.size())
        m_selected = new_selected;
    new_selected = m_selected;
    while (new_selected >= 0 && m_skills[new_selected]->getSelection() == Skill::Selection::kNotSelectable) {
        new_selected--;
    }
    if (new_selected >= 0)
        m_selected = new_selected;

    return m_skills[m_selected]->getSelection() != Skill::Selection::kNotSelectable;
}

}   // namespace views
}   // namespace gui
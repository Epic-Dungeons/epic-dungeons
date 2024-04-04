#include "skill.h"
#include "logging/logger.h"

namespace gui {
namespace views {

namespace {
static std::map<std::string, std::shared_ptr<graphics::Sprite>> sprites;

static std::shared_ptr<graphics::Sprite> getSprite(const std::string &path) {
    if (sprites.find(path) == sprites.end()) {
        sprites[path] = std::make_shared<graphics::Sprite>(path);
    }
    return sprites[path];
}
}   // namespace

Skill::Skill(const std::shared_ptr<engine::skills::Skill> &skill) : m_skill(skill) {
    m_sprite = getSprite("skills/" + skill->id + ".png");
}

void Skill::draw(const graphics::Renderer &renderer) const {
    if (!m_skill) {
        logging::error("Skill::draw: Skill is not bound");
        throw std::runtime_error("Skill::draw: Skill is not bound");
        return;
    }
    auto skill = m_skill;
    static std::map<Selection, graphics::Color> selection_colors = {
        {Selection::kNone, 0xffffff},
        {Selection::kSelected, 0xffffff},
        {Selection::kSelectable, 0x666666},
        {Selection::kNotSelectable, 0x222222},
    };

    const graphics::Color &selection_color = selection_colors.find(m_selection)->second;

    m_sprite->setOrigin(graphics::Sprite::Origin::TOP_LEFT).setColor(selection_color).setPosition(m_position);
    renderer.draw(m_sprite);
}

Skill &Skill::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

Skill &Skill::setSelection(const Selection &selection) {
    m_selection = selection;
    return *this;
}

const std::shared_ptr<Skill> Skill::getView(const std::shared_ptr<engine::skills::Skill> &skill) {
    auto view = ViewManager::getView(*skill);
    if (view)
        return std::dynamic_pointer_cast<Skill>(view);
    std::shared_ptr<Skill> new_view = std::make_shared<Skill>(skill);
    ViewManager::bind(*skill, new_view);
    return new_view;
}

Skill &Skill::setSize(const Vector2d &size) {
    m_sprite->setScale(size.x / m_sprite->getSize().x, size.y / m_sprite->getSize().y);
    return *this;
}

Vector2d Skill::getSize() const {
    return m_sprite->getSize();
}

Skill::Selection Skill::getSelection() const {
    return m_selection;
}

}   // namespace views
}   // namespace gui

#include "skill.h"
#include "engine/parsing/reader.h"
#include "logging/logger.h"
#include <string>

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
    auto split = skill->id.find_first_of('/');
    if (split == std::string::npos) {
        LOG_ERROR("Invalid skill id: {}, (has no owner, for now unknown)", skill->id);
        throw std::runtime_error("Invalid skill id: " + skill->id);
    }
    std::string owner = skill->id.substr(0, split);
    std::string name = skill->id.substr(split + 1);

    engine::parsing::FileDataPtr art_data =
        engine::parsing::FileReader::read("res/heroes/" + owner + "/" + owner + ".art.darkest");
    engine::parsing::DataRowPtr art_row =
        art_data->findRows("combat_skill", engine::parsing::StringParam("id", name)).front();
    if (!art_row) {
        LOG_ERROR("Skill art not found: {}", name);
        throw std::runtime_error("Skill art not found: " + name);
    }
    std::string icon = art_row->getString("icon");
    std::string icon_path = "res/heroes/" + owner + "/" + owner + ".ability." + icon + ".png";
    m_sprite = getSprite(icon_path);
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


#include "engine/engine.h"
#include "engine/skill.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "logging/logger.h"
#include <map>
#include <memory>

namespace gui {
namespace utils {

static std::map<std::string, std::shared_ptr<graphics::Sprite>> skill_sprites;

const std::shared_ptr<graphics::Sprite> getSkillSprite(const std::string &name) {
    if (skill_sprites.find(name) == skill_sprites.end()) {
        skill_sprites[name] = std::make_shared<graphics::Sprite>("skills/" + name + ".png");
    }
    return skill_sprites[name];
}

void drawSkills(const std::shared_ptr<graphics::Renderer> &renderer,
                const std::vector<std::shared_ptr<engine::skills::Skill>> &skills, const uint32_t &selected) {
    const float center = cfg::WINDOW_WIDTH / 2;
    const float cell_size = cfg::WINDOW_WIDTH / 15;
    const float margin_below = cell_size * 0.5;
    const float spacing = cell_size * 0.1;
    const uint8_t skills_size = skills.size();
    const float skills_width = skills_size * (cell_size + spacing) - spacing;
    const float queue_left = center - skills_width / 2;
    const float queue_top = cfg::WINDOW_HEIGHT - margin_below - cell_size;
    const float selected_margin = cell_size * 0.2;

    for (uint8_t i = 0; i < skills_size; i++) {
        auto skill = skills[i];
        std::shared_ptr<graphics::Sprite> skill_sprite = getSkillSprite(skill->id);
        skill_sprite->toSize(cell_size, cell_size);
        renderer->draw(skill_sprite.get()->setPosition({queue_left + i * (cell_size + spacing), queue_top}));
    }
    if (selected < skills_size) {
        auto skill = skills[selected];
        std::shared_ptr<graphics::Sprite> skill_sprite = getSkillSprite(skill->id);
        skill_sprite->toSize(cell_size + selected_margin, cell_size + selected_margin);
        renderer->draw(skill_sprite.get()->setPosition(
            {queue_left + selected * (cell_size + spacing) - selected_margin / 2, queue_top - selected_margin / 2}));
    }
}

}   // namespace utils
}   // namespace gui
#include "skill_info.h"
#include "logging/logger.h"
#include <stdexcept>

namespace gui {
namespace views {

SkillInfo &SkillInfo::bind(const std::shared_ptr<engine::skills::Skill> &skill) {
    m_skill = skill;
    return *this;
}

SkillInfo &SkillInfo::bindEntity(const std::shared_ptr<engine::entities::Entity> &entity) {
    if (entity == nullptr)
        m_entity.reset();
    else
        m_entity = entity;
    return *this;
}

SkillInfo &SkillInfo::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

void SkillInfo::draw(const graphics::Renderer &renderer) const {
    if (m_skill.expired()) {
        logging::error("SkillInfo::draw: Skill is expired");
        return;
    }

    auto skill = m_skill.lock();
    if (skill == nullptr) {
        logging::error("SkillInfo::draw: Skill is nullptr");
        return;
    }
    renderer.draw(graphics::Text(skill->name).setFont("story").setFontSize(50).setPosition(m_position));
    std::string description = "";
    auto combat = std::dynamic_pointer_cast<engine::skills::CombatSkill>(skill);
    if (combat != nullptr) {
        if (m_entity.expired()) {
            logging::error("SkillInfo::draw: Entity is expired");
            return;
        }
        auto weapon = m_entity.lock()->getWeapon();
        if (weapon == nullptr) {
            logging::error("SkillInfo::draw: Weapon is nullptr");
            return;
        }
        description += "Combat Skill\n";
        int32_t min_damage = round(weapon->minDamage * (100 + combat->damageMod) / 100);
        int32_t max_damage = round(weapon->maxDamage * (100 + combat->damageMod) / 100);
        int32_t crit_chance = weapon->criticalChanceMod + combat->criticalChanceMod;
        int32_t attack = weapon->attackMod + combat->attackMod;
        std::string launchable_positions = "";
        std::string targetable_positions = "";
        for (auto &pos : combat->launchablePositions)
            launchable_positions += std::to_string(pos) + " ";
        for (auto &pos : combat->targetablePositions)
            targetable_positions += std::to_string(pos) + " ";
        description += "Damage: " + std::to_string(min_damage) + "-" + std::to_string(max_damage) + "\n";
        description += "Crit: " + std::to_string(crit_chance) + "%\n";
        description += "Attack: " + std::to_string(attack) + "%\n";
        description += "Launch: " + launchable_positions + "\n";
        description += "Target: " + targetable_positions + "\n";
    }
    renderer.draw(graphics::Text(description).setFontSize(20).setPosition(m_position + Vector2d {0, 70}));
}

}   // namespace views
}   // namespace gui
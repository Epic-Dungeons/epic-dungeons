#include "hero.h"
#include "armor.h"
#include "reader.h"
#include "skill.h"
#include "weapon.h"
#include <string>

namespace engine {
namespace parsing {

/// @brief
/// @param path - path to heroes folder
/// @return
std::shared_ptr<entities::Hero> Hero::parse(const std::string &name, uint32_t level) {
    const std::string &path = "res/heroes/" + name + "/" + name + ".info.darkest";
    FileDataPtr info = FileReader::read(path);
    std::shared_ptr<entities::Hero> hero = std::make_shared<entities::Hero>();

    std::string armor_name = name + "_armour_" + std::to_string(level);
    std::string weapon_name = name + "_weapon_" + std::to_string(level);

    std::shared_ptr<items::Armor> armor =
        Armor::parse(info->findRows("armour", StringParam("name", armor_name)).front());
    std::shared_ptr<items::Weapon> weapon =
        Weapon::parse(info->findRows("weapon", StringParam("name", weapon_name)).front());

    LOG_DEBUG("Parsing hero: {}", path);
    // clang-format off
    hero->m_id = name;          LOG_DEBUG("\tID: {}", hero->m_id);
    hero->m_name = name;        LOG_DEBUG("\tName: {}", hero->m_name);
    hero->m_level = level;      LOG_DEBUG("\tLevel: {}", hero->m_level);
    // clang-format on
    auto resists = info->getRow("resistances");
    entities::Resistances res = {.stun = resists->getInt("stun"),
                                 .poison = resists->getInt("poison"),
                                 .bleed = resists->getInt("bleed"),
                                 .disease = resists->getInt("disease"),
                                 .move = resists->getInt("move"),
                                 .debuff = resists->getInt("debuff"),
                                 .death_blow = resists->getInt("death_blow"),
                                 .trap = resists->getInt("trap")};
    LOG_DEBUG("Resistances: stun={}, poison={}, bleed={}, disease={}, move={}, debuff={}, death_blow={}, trap={}",
              res.stun, res.poison, res.bleed, res.disease, res.move, res.debuff, res.death_blow, res.trap);
    hero->m_resistances = res;
    // clang-format off
    hero->setArmor(armor);      LOG_DEBUG("\tArmor: {}", hero->m_armor->name);
    hero->setWeapon(weapon);    LOG_DEBUG("\tWeapon: {}", hero->m_weapon->name);
    // clang-format on

    auto combat_skill_rows = info->findRows("combat_skill", IntParam("level", level));
    LOG_DEBUG("Skills: {}", combat_skill_rows.size());
    for (auto &row : combat_skill_rows) {
        hero->m_skills.push_back(parsing::CombatSkill::parse(row, name));
    }

    for (auto &tag : info->getRows("tag")) {
        hero->tags.push_back(tag->getString("id"));
    }

    // TODO: add camping and move skills
    return hero;
}

}   // namespace parsing
}   // namespace engine
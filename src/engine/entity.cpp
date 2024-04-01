#include "entity.h"
#include "item.h"
#include "logging/logger.h"
#include "skill.h"

namespace engine {
namespace entities {

Entity::Entity() : GameObject(), m_inventory() {
    m_inventory = std::make_shared<items::Storage>();   // Create a new storage for the entity
}

const int32_t Entity::calculateDamage(const std::shared_ptr<skills::CombatSkill> &skill) const {
    int32_t damage = 0;
    if (m_weapon) {
        damage = m_weapon->minDamage + (rand() % (m_weapon->maxDamage - m_weapon->minDamage + 1));
    }
    if (skill) {
        damage = (damage * (100 + skill->damageMod)) / 100;
    }
    return damage;
}

const int32_t Entity::calculateHitChance(const std::shared_ptr<entities::Entity> &target,
                                         const std::shared_ptr<skills::CombatSkill> &skill) const {
    int32_t hit_chance = 0;
    if (m_weapon)
        hit_chance += m_weapon->attackMod;
    if (skill)
        hit_chance += skill->attackMod;
    if (target->m_armor)
        hit_chance -= target->m_armor->defenseMod;

    return hit_chance;
}

const int32_t Entity::calculateCritChance(const std::shared_ptr<skills::CombatSkill> &skill) const {
    int32_t crit_chance = 0;
    if (m_weapon)
        crit_chance += m_weapon->criticalChanceMod;
    if (skill)
        crit_chance += skill->criticalChanceMod;

    return crit_chance;
}

const int32_t Entity::getSpeed() const {
    int32_t speed = 0;
    if (m_weapon)
        speed += m_weapon->speedMod;
    if (m_armor)
        speed += m_armor->speedMod;
    return speed;
}

const std::shared_ptr<items::Storage> &Entity::getInventory() const {
    return m_inventory;
}

const skills::AttackResult Entity::takeAttack(const std::shared_ptr<Entity> &attacker,
                                              const std::shared_ptr<skills::CombatSkill> &skill) {
    skills::AttackResult result;
    int32_t hit_chance = attacker->calculateHitChance(shared_from_this(), skill);
    if (rand() % 100 < hit_chance) {
        result.isHit = true;
        result.damage = attacker->calculateDamage(skill);
        if (rand() % 100 < attacker->calculateCritChance(skill)) {
            result.isCritical = true;
            result.damage *= 1.5;
        }

        result.damage = result.damage * (100 - m_armor->protectionMod) / 100;

        logging::debug(attacker->getName() + " attacks " + getName() + " with " + skill->name);
        updateHealth(-result.damage);
    }
    return result;
}

const uint8_t Entity::getPosition() const {
    if (m_party.expired()) {
        return 0;
    }
    return m_party.lock()->getMemberPosition(shared_from_this());
}

void Entity::updateHealth(const int32_t &amount) {
    int32_t old_health = m_health;
    m_health += amount;
    if (m_health > m_max_health) {
        m_health = m_max_health;
    }
    if (m_health <= 0) {
        m_health = 0;
        m_is_alive = false;
        if (!m_party.expired()) {
            m_party.lock()->memberDied(shared_from_this());
        }
    }
    if (m_health > 0) {
        m_is_alive = true;
    }
    logging::debug(m_name + " health: " + std::to_string(old_health) + " -> " + std::to_string(m_health));
}

const std::shared_ptr<items::Armor> &Entity::getArmor() const {
    return m_armor;
}

const std::shared_ptr<items::Weapon> &Entity::getWeapon() const {
    return m_weapon;
}

void Entity::setArmor(const std::shared_ptr<items::Armor> &armor) {
    if (!m_inventory->containsItem(armor)) {
        logging::warn("Entity " + m_name + " does not have armor " + armor->name);
        m_inventory->addItem(armor);
    }
    m_armor = armor;
}

void Entity::setWeapon(const std::shared_ptr<items::Weapon> &weapon) {
    if (!m_inventory->containsItem(weapon)) {
        logging::warn("Entity " + m_name + " does not have weapon " + weapon->name);
        m_inventory->addItem(weapon);
    }
    m_weapon = weapon;
}

const std::vector<std::shared_ptr<skills::Skill>> &Entity::getSkills() const {
    return m_skills;
}

const Resistances &Entity::getResistances() const {
    return m_resistances;
}

const std::shared_ptr<Party> Entity::getParty() const {
    return m_party.lock();
}

const std::string &Entity::getId() const {
    return m_id;
}

const std::string &Entity::getName() const {
    return m_name;
}

const int32_t Entity::getHealth() const {
    return m_health;
}

const int32_t Entity::getMaxHealth() const {
    return m_max_health;
}

const int32_t Entity::getDodge() const {
    int32_t dodge = 0;
    if (m_armor)
        dodge += m_armor->defenseMod;
    return dodge;
}

const int32_t Entity::getProtection() const {
    int32_t protection = 0;
    if (m_armor)
        protection += m_armor->protectionMod;
    return protection;
}

const int32_t Entity::getAttack() const {
    int32_t attack = 0;
    if (m_weapon)
        attack += m_weapon->attackMod;
    return attack;
}

const int32_t Entity::getCrit() const {
    int32_t crit = 0;
    if (m_weapon)
        crit += m_weapon->criticalChanceMod;
    return crit;
}

const int32_t Entity::getMinDamage() const {
    if (m_weapon)
        return m_weapon->minDamage;
    return 0;
}

const int32_t Entity::getMaxDamage() const {
    if (m_weapon)
        return m_weapon->maxDamage;
    return 0;
}

const bool Entity::isAlive() const {
    return m_is_alive;
}

}   // namespace entities
}   // namespace engine

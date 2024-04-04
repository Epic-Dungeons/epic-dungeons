#pragma once
#include "game_object.h"
#include "storage.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace engine {
namespace skills {
struct Skill;
struct AttackResult;
struct CombatSkill;
}   // namespace skills

namespace items {
struct Weapon;
struct Armor;
}   // namespace items

namespace entities {
class Party;

struct Resistances {
    int32_t stun = 30;   // 30% chance to resist
    int32_t poison = 30;
    int32_t bleed = 30;
    int32_t disease = 30;
    int32_t move = 30;
    int32_t debuff = 30;
    int32_t death_blow = 67;
    int32_t trap = 40;
};

class Entity : public GameObject, public std::enable_shared_from_this<Entity> {
    friend class Party;

public:
    Entity();

    virtual ~Entity() {}

    const bool isAlive() const;
    void setWeapon(const std::shared_ptr<items::Weapon> &weapon);
    const std::shared_ptr<items::Weapon> &getWeapon() const;
    void setArmor(const std::shared_ptr<items::Armor> &armor);
    const std::shared_ptr<items::Armor> &getArmor() const;
    void updateHealth(const int32_t &amount);

    template<typename skill>
    void addSkill() {
        m_skills.push_back(std::make_shared<skill>());
    }

    std::vector<std::shared_ptr<skills::Skill>> getSkills() const;

    virtual const skills::AttackResult takeAttack(const std::shared_ptr<Entity> &attacker,
                                                  const std::shared_ptr<skills::CombatSkill> &skill);

    virtual const int32_t calculateHitChance(const std::shared_ptr<Entity> &target,
                                             const std::shared_ptr<skills::Skill> &skill) const;

    virtual const int32_t calculateCritChance(const std::shared_ptr<skills::CombatSkill> &skill) const;

    virtual const int32_t calculateDamage(const std::shared_ptr<skills::CombatSkill> &skill) const;

    const int32_t getHealth() const;
    const int32_t getMaxHealth() const;
    const int32_t getSpeed() const;
    const int32_t getDodge() const;
    const int32_t getProtection() const;
    const int32_t getAttack() const;
    const int32_t getCrit() const;
    const int32_t getMinDamage() const;
    const int32_t getMaxDamage() const;

    const uint8_t getPosition() const;
    const std::string &getId() const;
    const std::string &getName() const;
    const Resistances &getResistances() const;
    const std::string getNameWithId() const;
    const std::shared_ptr<Party> getParty() const;
    const std::shared_ptr<items::Storage> &getInventory() const;

protected:
    std::string m_id;
    std::string m_name;
    bool m_is_alive = true;
    int32_t m_max_health = 20;
    int32_t m_health = 20;
    std::shared_ptr<items::Weapon> m_weapon = nullptr;
    std::shared_ptr<items::Armor> m_armor = nullptr;
    std::vector<std::shared_ptr<skills::Skill>> m_skills;
    Resistances m_resistances;
    std::weak_ptr<Party> m_party;
    std::shared_ptr<items::Storage> m_inventory;
};

class Party : public GameObject, public std::enable_shared_from_this<Party> {
    friend class Entity;

public:
    Party();

    void addMember(const std::shared_ptr<Entity> &member);
    void removeMember(const std::shared_ptr<Entity> &member);
    std::shared_ptr<Entity> getMember(const uint8_t &index) const;
    std::vector<std::shared_ptr<Entity>> getMembers() const;
    uint8_t getMembersCount() const;
    const uint8_t getAliveMembersCount() const;
    uint8_t getMemberPosition(const std::shared_ptr<const Entity> &member) const;
    void swapMembers(const uint8_t &index1, const uint8_t &index2);
    void swapMembers(const std::shared_ptr<Entity> &member1, const std::shared_ptr<Entity> &member2);
    bool hasMember(const std::shared_ptr<const Entity> &member) const;
    std::vector<std::shared_ptr<skills::Skill>> getSkills() const;
    void arrangeMembers();
    void clear();
    void memberDied(const std::shared_ptr<Entity> &member);
    std::shared_ptr<items::Storage> getInventory() const;

protected:
    std::vector<std::shared_ptr<Entity>> m_members;
    std::shared_ptr<items::Storage> m_inventory;
};

typedef std::shared_ptr<Entity> EntityPtr;

}   // namespace entities
}   // namespace engine

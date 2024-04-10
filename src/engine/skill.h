#pragma once
#include "game_object.h"
#include "item.h"
#include "storage.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace engine {

namespace skills {

struct Skill : public GameObject, public std::enable_shared_from_this<Skill> {
    Skill() = default;
    virtual ~Skill() = default;

    std::string id;
    std::string name;
    uint8_t level = 0;
    enum class Type { Melee, Ranged, None } type = Type::None;
    enum class Target { Single, Ranged, Random } target = Target::Single;
    enum class Side { Self, Party, OtherParty } side = Side::OtherParty;
    std::vector<uint8_t> launchablePositions;
    std::vector<uint8_t> targetablePositions;

    bool isUsable(const uint8_t position) const {
        return std::find(launchablePositions.begin(), launchablePositions.end(), position) != launchablePositions.end();
    }

    bool isTargetable(const uint8_t position) const {
        return std::find(targetablePositions.begin(), targetablePositions.end(), position) != targetablePositions.end();
    }
};

struct Move : public Skill {
    std::string id = "move";
    std::string name = "Move";
    uint8_t level = 0;
    uint8_t distance = 0;
    std::vector<uint8_t> launchablePositions = {1, 2, 3, 4};
    std::vector<uint8_t> targetablePositions = {1, 2, 3, 4};
};

struct CombatSkill : public Skill {
    float damageMod = 0;
    float attackMod = 0;
    float criticalChanceMod = 0;
    int8_t healMin = 0;
    int8_t healMax = 0;

    Type type = Type::Melee;
};

struct AttackResult {
    bool isHit = false;
    bool isCritical = false;
    int32_t damage = 0;
};

struct ConsumeItemSkill : public Skill {
    ConsumeItemSkill(std::shared_ptr<items::Consumable> consumable, std::shared_ptr<items::Storage> storage) :
        consumable(consumable), storage(storage) {
        id = consumable->id;
        name = consumable->name;
        level = 0;
        type = Type::None;
        target = Target::Single;
        side = Side::Self;
        launchablePositions = {1, 2, 3, 4};
        targetablePositions = {1, 2, 3, 4};
    }

    std::shared_ptr<items::Consumable> consumable;
    std::shared_ptr<items::Storage> storage;
};

using Type = Skill::Type;
using Target = Skill::Target;
using Side = Skill::Side;

}   // namespace skills
}   // namespace engine

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

enum class Type { kMelee, kRanged, kMove };

enum class TargetType {
    kSelf,
    kIndividual,
    kParty,
    kOtherParty,
};

struct Skill : public GameObject, public std::enable_shared_from_this<Skill> {
    Skill() = default;
    virtual ~Skill() = default;

    std::string id;
    std::string name;
    uint8_t level = 0;
    Type type = Type::kMelee;
    TargetType targetType = TargetType::kIndividual;
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
    Type type = Type::kMove;
    TargetType targetType = TargetType::kIndividual;

    uint8_t distance = 0;
};

struct CombatSkill : public Skill {
    float damageMod = 0;
    float attackMod = 0;
    float criticalChanceMod = 0;
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
        targetType = TargetType::kSelf;
        launchablePositions = {1, 2, 3, 4};
        targetablePositions = {1, 2, 3, 4};
    }

    std::shared_ptr<items::Consumable> consumable;
    std::shared_ptr<items::Storage> storage;
};

}   // namespace skills
}   // namespace engine

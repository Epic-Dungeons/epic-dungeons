#pragma once
#include "game_object.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace engine {

namespace skills {

enum class Type {
    kMelee,
    kRanged,
    kMove,
};

enum class TargetType {
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

}   // namespace skills
}   // namespace engine

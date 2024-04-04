#pragma once
#include "engine/skill.h"

namespace engine {
namespace skills {
struct FlashingDaggers : public CombatSkill {
    FlashingDaggers() : CombatSkill() {
        id = "flashing_daggers";
        name = "Flashing Daggers";
        level = 0;
        type = Type::kRanged;
        targetType = TargetType::kIndividual;
        launchablePositions = {4, 3, 2};
        targetablePositions = {2, 3};
        attackMod = 90;
        damageMod = -33;
        criticalChanceMod = -35;
    }
};

}   // namespace skills
}   // namespace engine

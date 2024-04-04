#pragma once
#include "engine/skill.h"

namespace engine {
namespace skills {
struct Pick : public CombatSkill {
    Pick() : CombatSkill() {
        id = "heal";
        name = "Bandage";
        level = 0;
        type = Type::kMelee;
        targetType = TargetType::kParty;
        launchablePositions = {4, 3, 2, 1};
        targetablePositions = {1, 2, 3, 4};
        attackMod = +100;
        damageMod = -100;
        criticalChanceMod = -100;
    }
};

}   // namespace skills
}   // namespace engine

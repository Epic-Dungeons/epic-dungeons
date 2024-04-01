#pragma once
#include "hero.h"

#include "engine/armor/grave_robber.h"
#include "engine/weapons/pick_and_knives.h"

#include "engine/attacks/flashing_daggers.h"
#include "engine/attacks/pick.h"

namespace engine {
namespace entities {

class GraveRobber : public Hero {
public:
    GraveRobber() : Hero() {
        m_id = "grave_robber";
        m_name = "Grave Robber";
        m_health = 40;
        m_max_health = 40;
        setWeapon(std::make_shared<items::PickAndKnives>());
        setArmor(std::make_shared<items::GraveRobberArmor>());
        addSkill<skills::Pick>();
        addSkill<skills::FlashingDaggers>();
    }
};

}   // namespace entities
}   // namespace engine

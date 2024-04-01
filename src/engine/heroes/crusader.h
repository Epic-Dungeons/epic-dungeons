#pragma once
#include "hero.h"

#include "engine/armor/crusader.h"
#include "engine/weapons/longsword.h"

#include "engine/attacks/smite.h"
#include "engine/attacks/zealous_accusation.h"

namespace engine {
namespace entities {

class Crusader : public Hero {
public:
    Crusader() : Hero() {
        m_id = "crusader";
        m_name = "Crusader";
        m_health = 40;
        m_max_health = 40;
        setWeapon(std::make_shared<items::Longsword>());
        setArmor(std::make_shared<items::CrusaderArmor>());
        addSkill<skills::Smite>();
        addSkill<skills::ZealousAccusation>();
    }
};

}   // namespace entities
}   // namespace engine

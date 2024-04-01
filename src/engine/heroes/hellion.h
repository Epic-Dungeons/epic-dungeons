#pragma once
#include "hero.h"

#include "engine/armor/hellion.h"
#include "engine/weapons/glaive.h"

#include "engine/attacks/iron_swan.h"
#include "engine/attacks/wicked_hack.h"

namespace engine {
namespace entities {

class Hellion : public Hero {
public:
    Hellion() : Hero() {
        m_id = "hellion";
        m_name = "Hellion";
        m_health = 26;
        m_max_health = 26;
        setWeapon(std::make_shared<items::Glaive>());
        setArmor(std::make_shared<items::HellionArmor>());
        addSkill<skills::WickedHack>();
        addSkill<skills::IronSwan>();
    }
};

}   // namespace entities
}   // namespace engine

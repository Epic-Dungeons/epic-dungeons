#pragma once
#include "hero.h"

#include "engine/armor/arbalest.h"
#include "engine/weapons/crossbow.h"

#include "engine/attacks/sniper_shot.h"
#include "engine/attacks/suppresing_fire.h"

namespace engine {
namespace entities {

class Arbalest : public Hero {
public:
    Arbalest() : Hero() {
        m_id = "arbalest";
        m_name = "Arbalest";
        m_health = 27;
        m_max_health = 27;
        setWeapon(std::make_shared<items::Crossbow>());
        setArmor(std::make_shared<items::ArbalestArmor>());
        addSkill<skills::SniperShot>();
        addSkill<skills::SuppresingFire>();
    }
};

}   // namespace entities
}   // namespace engine

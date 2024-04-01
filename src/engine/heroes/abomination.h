#pragma once
#include "hero.h"

#include "engine/armor/abomination.h"
#include "engine/weapons/incubating_mutation.h"

#include "engine/attacks/manacles.h"
#include "engine/attacks/vomit.h"

namespace engine {
namespace entities {

class Abomination : public Hero {
public:
    Abomination() : Hero() {
        m_id = "abomination";
        m_name = "Abomination";
        m_health = 26;
        m_max_health = 26;
        setWeapon(std::make_shared<items::IncubatingMutation>());
        setArmor(std::make_shared<items::AbominationArmor>());
        addSkill<skills::Manacles>();
        addSkill<skills::Vomit>();
    }
};

}   // namespace entities
}   // namespace engine

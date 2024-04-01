#pragma once
#include "hero.h"

#include "engine/armor/bounty_hunter.h"
#include "engine/weapons/abandoned_axe.h"

#include "engine/attacks/collect_bounty.h"
#include "engine/attacks/target_tag.h"

namespace engine {
namespace entities {

class BountyHunter : public Hero {
public:
    BountyHunter() : Hero() {
        m_id = "bounty_hunter";
        m_name = "Bounty Hunter";
        m_health = 25;
        m_max_health = 25;
        setWeapon(std::make_shared<items::AbandonedAxe>());
        setArmor(std::make_shared<items::BountyHunterArmor>());
        addSkill<skills::CollectBounty>();
        addSkill<skills::TargetTag>();
    }
};

}   // namespace entities
}   // namespace engine

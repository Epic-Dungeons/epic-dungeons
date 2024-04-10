#pragma once
#include "hero.h"

namespace engine {
namespace entities {

class BountyHunter : public Hero {
public:
    BountyHunter() : Hero("bounty_hunter") {};
};

}   // namespace entities
}   // namespace engine

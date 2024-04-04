#pragma once

#include "entity.h"
#include "heroes/abomination.h"
#include "heroes/antiquarian.h"
#include "heroes/arbalest.h"
#include "heroes/bounty_hunter.h"
#include "heroes/crusader.h"
#include "heroes/grave_robber.h"
#include "heroes/hellion.h"
#include "heroes/highwayman.h"

namespace engine {
namespace entities {

static std::vector<std::shared_ptr<Entity>> createEnemies() {
    std::vector<std::shared_ptr<Entity>> enemies;
    for (int i = 0; i < 3; i++) {
        // clang-format off
        std::vector<std::shared_ptr<Entity>> enemy_pool = {
            std::make_shared < Abomination > (),
            std::make_shared < Antiquarian > (),
            std::make_shared < Arbalest > (),
            std::make_shared < BountyHunter > (),
            std::make_shared < Crusader > (),
            std::make_shared < GraveRobber > (),
            std::make_shared < Hellion > (),
            std::make_shared < Highwayman > ()
        };
        // clang-format on
        enemies.push_back(enemy_pool[rand() % enemy_pool.size()]);
    }

    return enemies;
}

}   // namespace entities
}   // namespace engine
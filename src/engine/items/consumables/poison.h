#pragma once
#include "engine/entity.h"
#include "engine/item.h"
#include <logging/logger.h>
#include <memory>

namespace engine {
namespace items {

struct Poison : public Consumable {
    Poison() : Consumable() {
        id = "poison";
        name = "Poison";
        description = "Deals 10 damage.";
        healthMod = -10;
    }

    void use(const std::shared_ptr<entities::Entity> &entity) override {
        logging::info("Poisoning entity: " + entity->getId() + " by " + std::to_string(healthMod) + " HP.");
        entity->updateHealth(healthMod);
    }
};

}   // namespace items
}   // namespace engine

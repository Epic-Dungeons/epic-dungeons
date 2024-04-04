#pragma once
#include "engine/entity.h"
#include "engine/item.h"
#include <logging/logger.h>
#include <memory>

namespace engine {
namespace items {

struct Bandage : public Consumable {
    Bandage() : Consumable() {
        id = "bandage";
        name = "Bandage";
        description = "Heals 10 HP.";
        healthMod = 10;
    }

    void use(const std::shared_ptr<entities::Entity> &entity) override {
        logging::info("Healing entity: " + entity->getId() + " by " + std::to_string(healthMod) + " HP.");
        entity->updateHealth(healthMod);
    }
};

}   // namespace items
}   // namespace engine

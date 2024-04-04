#include "chest.h"
#include "engine/items/consumables/bandage.h"
#include "engine/items/golden_coin.h"

namespace engine {

Chest::Chest() : GameObject() {
    m_storage = std::make_shared<items::Storage>();
}

const std::shared_ptr<items::Storage> &Chest::getStorage() const {
    return m_storage;
}

std::shared_ptr<Chest> createChest() {
    auto chest = std::make_shared<Chest>();
    // generate random items
    // gold from 3 to 10
    uint32_t gold_amount = 3 + rand() % 8;
    chest->getStorage()->addItem(std::make_shared<items::GoldenCoin>(), gold_amount);
    // health potion
    static const float health_potion_chance = 0.5;
    if (rand() % 100 < health_potion_chance * 100)
        chest->getStorage()->addItem(std::make_shared<items::Bandage>());
    return chest;
}

}   // namespace engine
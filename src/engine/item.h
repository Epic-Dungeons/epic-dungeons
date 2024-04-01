#pragma once
#include "game_object.h"
#include <memory>
#include <string>

namespace engine {

namespace entities {

class Entity;

}   // namespace entities

namespace items {

struct Item : public GameObject {
    Item() : GameObject() {}

    std::string id;
    std::string name;
    std::string description = "";

    bool isStackable = false;
    int32_t stackSize = 1;

    virtual ~Item() {};
};

struct Consumable : public Item {
    Consumable() : Item() {
        isStackable = true;
    }

    int32_t healthMod = 0;
    int32_t manaMod = 0;
    int32_t attackMod = 0;
    int32_t defenseMod = 0;
    int32_t protectionMod = 0;
    int32_t speedMod = 0;

    int32_t duration = 0;

    virtual void use(const std::shared_ptr<entities::Entity> &entity) = 0;
};

struct Weapon : public Item {
    Weapon() : Item() {
        isStackable = false;
    }

    int32_t attackMod = 0;
    int32_t minDamage = 0;
    int32_t maxDamage = 0;
    int32_t criticalChanceMod = 0;
    int32_t speedMod = 0;
};

struct Armor : public Item {
    Armor() : Item() {
        isStackable = false;
    }

    int32_t defenseMod = 0;
    int32_t protectionMod = 0;
    int32_t speedMod = 0;
};

typedef std::shared_ptr<Item> ItemPtr;

}   // namespace items
}   // namespace engine

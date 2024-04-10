#include "weapon.h"

namespace engine {
namespace parsing {

std::shared_ptr<items::Weapon> Weapon::parse(const DataRowPtr &data) {
    std::shared_ptr<items::Weapon> weapon = std::make_shared<items::Weapon>();
    weapon->id = data->getString("name");
    weapon->name = data->getString("name");
    weapon->attackMod = data->getFloat("atk") * 100.0f;
    weapon->minDamage = data->getInt("dmg", 0);
    weapon->maxDamage = data->getInt("dmg", 1);
    weapon->criticalChanceMod = data->getFloat("crit") * 100.0f;
    weapon->speedMod = data->getInt("spd");
    return weapon;
}

}   // namespace parsing
}   // namespace engine
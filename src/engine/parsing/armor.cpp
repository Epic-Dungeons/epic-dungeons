#include "armor.h"

namespace engine {
namespace parsing {

std::shared_ptr<items::Armor> Armor::parse(const DataRowPtr &data) {
    std::shared_ptr<items::Armor> armor = std::make_shared<items::Armor>();
    armor->id = data->getString("name");
    armor->name = data->getString("name");
    armor->defenseMod = data->getFloat("def") * 100.0f;
    armor->protectionMod = data->getInt("prot");
    armor->speedMod = data->getInt("spd");
    armor->healthMod = data->getInt("hp");
    return armor;
}

}   // namespace parsing
}   // namespace engine
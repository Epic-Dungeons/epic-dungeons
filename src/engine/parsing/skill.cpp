#include "skill.h"

namespace engine {
namespace parsing {

std::shared_ptr<skills::CombatSkill> CombatSkill::parse(const DataRowPtr &data) {
    auto skill = std::make_shared<skills::CombatSkill>();
    skill->id = data->getParameter("id")->getValue();
    std::string name = skill->id;
    name[0] = std::toupper(name[0]);
    for (auto &c : name) {
        if (c == '_')
            c = ' ';
    }
    skill->name = name;
    skill->level = data->getParameter("level")->getValue<int32_t>();
    std::string type = data->getParameter("type")->getValue();
    if (type == "melee")
        skill->type = skills::Type::kMelee;
    else if (type == "ranged")
        skill->type = skills::Type::kRanged;
    else if (type == "move")
        skill->type = skills::Type::kMove;
    else {
        skill->type = skills::Type::kMelee;
        LOG_WARNING("Unknown skill type: " + type);
    }
    skill->attackMod = data->getParameter("atk")->getValue<float>() * 100.0f;
    skill->damageMod = data->getParameter("dmg")->getValue<float>() * 100.0f;
    skill->criticalChanceMod = data->getParameter("crit")->getValue<float>() * 100.0f;
    std::string launchablePositions = data->getParameter("launch")->getValue();
    std::vector<uint8_t> launchable;
    for (auto &c : launchablePositions) {
        if ('0' <= c && c <= '9')
            launchable.push_back(c - '0');
        else {
            LOG_ERROR("Invalid launchable position: " + std::string(1, c));
            exit(1);
        }
    }
    skill->launchablePositions = launchable;
    std::string targetablePositions = data->getParameter("target")->getValue();
    skill->targetType = skills::TargetType::kIndividual;
    if (targetablePositions[0] == '~') {
        skill->targetType = skills::TargetType::kOtherParty;
        targetablePositions = targetablePositions.substr(1);
    }

    std::vector<uint8_t> targetable;
    for (auto &c : targetablePositions) {
        if ('0' <= c && c <= '9')
            targetable.push_back(c - '0');
        else {
            LOG_ERROR("Invalid targetable position: " + std::string(1, c));
            exit(1);
        }
    }
    skill->targetablePositions = targetable;

    return skill;
}

}   // namespace parsing
}   // namespace engine
#include "skill.h"

namespace engine {
namespace parsing {

namespace {
std::string typeToString(skills::Type type) {
    switch (type) {
        case skills::Type::Melee:
            return "Melee";
        case skills::Type::Ranged:
            return "Ranged";
        case skills::Type::None:
            return "None";
    }
}

std::string targetToString(skills::Target target) {
    switch (target) {
        case skills::Target::Single:
            return "Single";
        case skills::Target::Ranged:
            return "Ranged";
        case skills::Target::Random:
            return "Random";
    }
}

std::string sideToString(skills::Side side) {
    switch (side) {
        case skills::Side::Self:
            return "Self";
        case skills::Side::Party:
            return "Party";
        case skills::Side::OtherParty:
            return "OtherParty";
    }
}

template<typename T>
std::string vectorToString(const std::vector<T> &vec) {
    std::string result = "[";
    for (auto &v : vec) {
        result += std::to_string(v) + ", ";
    }
    if (!vec.empty())
        result.pop_back();
    if (!vec.empty())
        result.pop_back();
    result += "]";
    return result;
}
}   // namespace

std::shared_ptr<skills::CombatSkill> CombatSkill::parse(const DataRowPtr &data, const std::string &prefix) {
    auto skill = std::make_shared<skills::CombatSkill>();
    std::string id = data->getString("id");
    LOG_DEBUG("Parsing skill: {}", id);
    if (prefix.empty())
        skill->id = id;
    else
        // Concatenate the prefix and the id (e.g. "prefix/id"
        skill->id = prefix + "/" + id;
    LOG_DEBUG("\tID: {}", skill->id);
    std::string name = id;
    name[0] = std::toupper(name[0]);
    for (auto &c : name) {
        if (c == '_')
            c = ' ';
    }
    skill->name = name;
    LOG_DEBUG("\tName: {}", skill->name);
    skill->level = data->getInt("level");
    LOG_DEBUG("\tLevel: {}", skill->level);
    std::string type = data->getString("type");
    if (type == "melee")
        skill->type = skills::Type::Melee;
    else if (type == "ranged")
        skill->type = skills::Type::Ranged;
    else {
        skill->type = skills::Type::None;
        if (data->hasParameter("type"))
            LOG_ERROR("Invalid skill type: " + type);
    }
    LOG_DEBUG("\tType: {}", typeToString(skill->type));
    skill->attackMod = data->getFloat("atk") * 100.0f;
    skill->damageMod = data->getInt("dmg") * 100.0f;
    skill->criticalChanceMod = data->getFloat("crit") * 100.0f;
    skill->healMin = data->getInt("heal", 0);
    skill->healMax = data->getInt("heal", 0);
    LOG_DEBUG("\tATK: {}%", skill->attackMod);
    LOG_DEBUG("\tDMG: {}%", skill->damageMod);
    LOG_DEBUG("\tCRIT: {}%", skill->criticalChanceMod);
    LOG_DEBUG("\tHeal: {}-{}", skill->healMin, skill->healMax);

    std::string launchablePositions = data->getString("launch");
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
    LOG_DEBUG("\tLaunchable: {}", vectorToString(skill->launchablePositions));
    std::string targetablePositions = data->getString("target");
    std::vector<uint8_t> targetable;
    skill->target = skills::Target::Single;   // default
    skill->side = skills::Side::OtherParty;   // default
    for (auto &c : targetablePositions) {
        if (c == '~')
            skill->target = skills::Target::Ranged;
        else if (c == '@')
            skill->side = skills::Side::Party;
        else if (c == '?')
            skill->target = skills::Target::Random;
        else if ('0' <= c && c <= '9')
            targetable.push_back(c - '0');
        else {
            LOG_ERROR("Invalid targetable position: " + std::string(1, c));
            exit(1);
        }
    }
    if (targetable.empty())
        skill->side = skills::Side::Self;
    skill->targetablePositions = targetable;
    LOG_DEBUG("\tTargetable: {}", vectorToString(skill->targetablePositions));
    LOG_DEBUG("\tTarget: {}", targetToString(skill->target));
    LOG_DEBUG("\tSide: {}", sideToString(skill->side));

    return skill;
}

}   // namespace parsing
}   // namespace engine
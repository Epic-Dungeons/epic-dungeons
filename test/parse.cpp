
#include "engine/parsing/reader.h"
#include "engine/parsing/skill.h"
#include <iostream>
#include <string>

int main() {
    std::string path = "res/heroes/grave_robber/grave_robber.info.darkest";

    std::shared_ptr<engine::parsing::FileData> data = engine::parsing::FileReader::read(path);

    auto level0_rows = data->findRows("combat_skill", engine::parsing::IntParam("level", 0));
    for (const auto &row : level0_rows) {
        auto skill = engine::parsing::CombatSkill::parse(row);
        std::cout << "Skill: " << skill->name << std::endl;
        std::cout << "Level: " << (int) skill->level << std::endl;
        std::cout << "Type: " << (int) skill->type << std::endl;
        std::cout << "Attack mod: " << skill->attackMod << std::endl;
        std::cout << "Damage mod: " << skill->damageMod << std::endl;

        std::cout << "Launchable positions: ";
        for (const auto &pos : skill->launchablePositions) {
            std::cout << (int) pos << " ";
        }
        std::cout << std::endl;

        std::cout << "Targetable positions: ";
        for (const auto &pos : skill->targetablePositions) {
            std::cout << (int) pos << " ";
        }
        std::cout << std::endl;

        std::cout << std::endl;
    }

    return 0;
}
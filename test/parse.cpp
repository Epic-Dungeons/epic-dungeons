
#include "engine/parsing/hero.h"
#include "engine/parsing/reader.h"
#include "engine/parsing/skill.h"
#include <glob.h>
#include <iostream>
#include <string>

std::vector<std::string> glob(const std::string &pattern) {
    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    std::vector<std::string> files;
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

void parse_skills(const std::string &path) {
    std::shared_ptr<engine::parsing::FileData> data = engine::parsing::FileReader::read(path);

    std::vector<engine::parsing::DataRowPtr> level0_rows =
        data->findRows("combat_skill", engine::parsing::IntParam("level", 0));

    for (const auto &row : level0_rows) {
        std::shared_ptr<engine::skills::CombatSkill> skill = engine::parsing::CombatSkill::parse(row);
        std::cout << "Skill: " << skill->name << std::endl;
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") {
            logging::setLevel(spdlog::level::debug);
            logging::debug("Verbose mode enabled");
        }
        if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
            std::cout << "Usage: " << argv[0] << " [-v|--verbose] [-h|--help]" << std::endl;
            return 0;
        }
    }

    for (const auto &path : glob("res/heroes/*")) {
        std::string hero_name = path.substr(path.find_last_of('/') + 1);
        LOG_DEBUG("Parsing hero: res/heroes/{}/{}.info.darkest", hero_name, hero_name);
        std::shared_ptr<engine::entities::Hero> hero = engine::parsing::Hero::parse(hero_name);
    }

    return 0;
}
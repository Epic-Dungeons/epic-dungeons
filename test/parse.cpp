
#include "engine/parsing/reader.h"
#include <iostream>
#include <string>

int main() {
    std::string path = "res/data/heroes/grave_robber.info.darkest";

    std::shared_ptr<engine::parsing::FileData> data = engine::parsing::FileReader::read(path);

    auto level0_rows = data->findRows("combat_skill", engine::parsing::IntParam("level", 0));
    for (const auto &row : level0_rows) {
        std::cout << row->getParameter("id")->getValue() << std::endl;
    }

    return 0;
}
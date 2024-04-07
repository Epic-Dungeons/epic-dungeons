#include "reader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace engine {
namespace parsing {
/* ----------------- DataParameter ----------------- */

DataParameter::DataParameter(const std::string &rawData) : name(""), values() {
    /* sample data
    .name "grave robber"
    */
    std::istringstream stream(rawData);
    std::string token;
    stream >> token;
    name = token.substr(1);   // .name -> name

    std::string value;
    while (stream >> token) {
        if (value.empty())
            value = token;
        else
            value = value + " " + token;
        if (token[0] == '"' && token[token.size() - 1] != '"')
            continue;
        if (token[token.size() - 1] == '"')
            value = value.substr(1, value.size() - 2);   // "grave robber" -> grave robber
        values.push_back(value);
        value.clear();
    }

    if (!value.empty())
        values.push_back(value);
}

uint32_t DataParameter::size() const {
    return values.size();
}

/* ----------------- DataRow ----------------- */

DataRow::DataRow(const std::string &rawData) : name(""), parameters() {
    /* sample data
    resistances: .stun 20% .poison 50% .bleed 30% .disease 30% .move 20% .debuff 30% .death_blow 67% .trap 50%
    */
    std::istringstream stream(rawData);
    std::string token;
    stream >> token;
    name = token.substr(0, token.size() - 1);   // resistances: -> resistances

    std::string parameterData;
    while (stream >> token) {
        if (token[0] == '.') {
            if (!parameterData.empty()) {
                std::shared_ptr<DataParameter> param = std::make_shared<DataParameter>(parameterData);
                parameters[param->name] = param;
            }
            parameterData = token;
        } else {
            parameterData = parameterData + " " + token;
        }
    }
    if (!parameterData.empty()) {
        std::shared_ptr<DataParameter> param = std::make_shared<DataParameter>(parameterData);
        parameters[param->name] = param;
    }
}

std::shared_ptr<DataParameter> DataRow::getParameter(const std::string &name) const {
    return parameters.at(name);
}

std::vector<std::string> DataRow::getParameterNames() const {
    std::vector<std::string> names;
    for (auto &parameter : parameters)
        names.push_back(parameter.first);
    return names;
}

uint32_t DataRow::size() const {
    return parameters.size();
}

/* ----------------- FileData ----------------- */

FileData::FileData(const std::string &rawData) : rows() {
    /* sample data
    resistances: .stun 20% .poison 50% .bleed 30% .disease 30% .move 20% .debuff 30% .death_blow 67% .trap 50%
    combat_skill: .id "pick" .level 0 .type "melee" .atk 90% .dmg -15% .crit 1% .launch 321 .target 12 .is_crit_valid True .ignore_protection true
    */
    std::istringstream stream(rawData);
    std::string row;
    while (std::getline(stream, row)) {
        if (row.empty())
            continue;
        rows.push_back(std::make_shared<DataRow>(row));
    }
}

const std::shared_ptr<DataRow> &FileData::getRow(uint32_t index) const {
    return rows.at(index);
}

const std::shared_ptr<DataRow> &FileData::getRow(const std::string &name) const {
    for (auto &row : rows) {
        if (row->name == name)
            return row;
    }
    throw std::runtime_error("Row not found");
}

const std::vector<std::shared_ptr<DataRow>> &FileData::getRows() const {
    return rows;
}

const std::vector<std::shared_ptr<DataRow>> FileData::getRows(const std::string &name) const {
    std::vector<std::shared_ptr<DataRow>> result;
    for (auto &row : rows) {
        if (row->name == name)
            result.push_back(row);
    }
    return result;
}

/* ----------------- FileReader ----------------- */

std::shared_ptr<FileData> FileReader::read(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("File not found");

    std::string rawData;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty())
            continue;
        rawData = rawData + line + "\n";
    }
    return std::make_shared<FileData>(rawData);
}

}   // namespace parsing
}   // namespace engine

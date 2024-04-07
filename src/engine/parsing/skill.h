#pragma once

#include "engine/skill.h"
#include "reader.h"
#include <memory>
#include <string>

namespace engine {
namespace parsing {

class CombatSkill {
public:
    static std::shared_ptr<skills::CombatSkill> parse(const DataRowPtr &data);
};

}   // namespace parsing
}   // namespace engine
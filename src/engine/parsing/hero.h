#pragma once
#include "engine/game_object.h"
#include "engine/heroes/hero.h"
#include "reader.h"
#include <string>
#include <vector>

namespace engine {
namespace parsing {
class Hero {
public:
    Hero() = delete;
    static std::shared_ptr<entities::Hero> parse(const std::string &name, uint32_t level = 0);
};
}   // namespace parsing
}   // namespace engine
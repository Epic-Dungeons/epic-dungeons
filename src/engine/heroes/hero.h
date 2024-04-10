#pragma once

#include "engine/entity.h"
#include <memory>

namespace engine {
namespace parsing {
class Hero;
}   // namespace parsing

namespace entities {
class Hero : public Entity {
    friend class parsing::Hero;

public:
    Hero();
    Hero(const std::string &id);
};
}   // namespace entities
}   // namespace engine

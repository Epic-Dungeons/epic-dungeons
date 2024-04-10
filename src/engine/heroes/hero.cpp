#include "hero.h"
#include "engine/parsing/hero.h"

namespace engine {
namespace entities {
Hero::Hero() : Entity() {}

Hero::Hero(const std::string &id) {
    *this = *parsing::Hero::parse(id);
}

}   // namespace entities
}   // namespace engine
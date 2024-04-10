#pragma once

#include "engine/item.h"
#include "reader.h"
#include <memory>
#include <string>
#include <vector>

namespace engine {
namespace parsing {

class Weapon {
public:
    static std::shared_ptr<items::Weapon> parse(const DataRowPtr &data);
};

}   // namespace parsing
}   // namespace engine
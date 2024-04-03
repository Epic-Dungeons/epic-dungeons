#pragma once
#include "engine/item.h"
#include "logging/logger.h"
#include <memory>

namespace engine {
namespace items {

struct GoldenCoin : public Item {
    GoldenCoin() : Item() {
        id = "golden_coin";
        name = "Golden Coin";
        description = "Makes you rich.";
        isStackable = true;
    }
};

}   // namespace items
}   // namespace engine

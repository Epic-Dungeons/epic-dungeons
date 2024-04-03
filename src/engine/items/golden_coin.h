#pragma once
#include "engine/item.h"
#include "logging/logger.h"
#include <memory>

namespace engine {
namespace items {

struct GoldenCoin : public Item {
    GoldenCoin(const uint32_t &amount = 1) : Item() {
        id = "golden_coin";
        name = "Golden Coin";
        description = "Makes you rich.";
        isStackable = true;
        if (amount == 0) {
            logging::error("GoldenCoin: amount cannot be 0.");
            throw std::invalid_argument("GoldenCoin: amount cannot be 0.");
        }
        stackSize = amount;
    }
};

}   // namespace items
}   // namespace engine

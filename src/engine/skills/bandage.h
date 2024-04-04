#pragma once
#include "engine/items/consumables/bandage.h"
#include "engine/skill.h"
#include "engine/storage.h"

namespace engine {
namespace skills {
struct Bandage : public ConsumeItemSkill {
    Bandage(const std::shared_ptr<engine::items::Bandage> &bandage,
            const std::shared_ptr<engine::items::Storage> &storage) :
        ConsumeItemSkill(bandage, storage) {}
};

}   // namespace skills
}   // namespace engine

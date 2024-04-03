#pragma once
#include "logging/logger.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace engine {
namespace {
size_t object_counter = 0;
}   // namespace

class GameObject {
public:
    GameObject() : objectId(object_counter++) {}

    GameObject(const GameObject &other) : objectId(object_counter++) {}

    GameObject &operator=(const GameObject &other) {
        if (this == &other) {
            return *this;
        }
        const_cast<size_t &>(objectId) = other.objectId;
        return *this;
    }

    virtual ~GameObject() = default;

    bool operator==(const GameObject &other) const {
        return objectId == other.objectId;
    }

    bool operator!=(const GameObject &other) const {
        return objectId != other.objectId;
    }

    const size_t objectId = 0;
};

}   // namespace engine
#pragma once
#include "game_object.h"
#include "storage.h"

namespace engine {
class Chest : public GameObject {
public:
    Chest();
    const std::shared_ptr<items::Storage> &getStorage() const;

protected:
    virtual void onOpen() {};
    virtual void onClose() {};

private:
    std::shared_ptr<items::Storage> m_storage;
};

std::shared_ptr<Chest> createChest();

}   // namespace engine
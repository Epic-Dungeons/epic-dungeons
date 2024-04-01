#include "storage.h"
#include "logging/logger.h"

namespace engine {
namespace items {

std::shared_ptr<Item> Storage::getItem(const std::string &id) const {
    for (auto &item : m_items) {
        if (item->id == id) {
            return item;
        }
    }
    return nullptr;
}

bool Storage::addItem(const std::shared_ptr<Item> &item) {
    if (m_max_items != 0 && m_items.size() == m_max_items) {
        logging::warn("Storage is full");
        return false;
    }
    if (item->isStackable) {
        ItemPtr m_item = getItem(item->id);
        if (m_item) {
            logging::info("Adding item to stack: " + item->id);
            m_item->stackSize += item->stackSize;
        } else {
            m_items.insert(item);
        }
    } else {
        m_items.insert(item);
    }
    return true;
}

std::vector<std::shared_ptr<Item>> Storage::getItems() const {
    return {m_items.begin(), m_items.end()};
}

std::vector<std::shared_ptr<Item>> Storage::getItems(const std::string &id) const {
    std::vector<std::shared_ptr<Item>> items;
    for (auto &item : m_items) {
        if (item->id == id) {
            items.push_back(item);
        }
    }
    return items;
}

bool Storage::containsItem(const std::shared_ptr<Item> &item) const {
    return m_items.find(item) != m_items.end();
}

bool Storage::removeItem(const std::shared_ptr<Item> &item) {
    logging::debug("Removing item: " + item->id);
    auto it = m_items.find(item);
    if (it != m_items.end()) {
        m_items.erase(it);
        return true;
    }
    return false;
}

bool Storage::removeOneItem(const std::shared_ptr<Item> &item) {
    logging::debug("Removing one item: " + item->id);
    auto it = m_items.find(item);
    if (it != m_items.end()) {
        if ((*it)->stackSize > 1) {
            (*it)->stackSize--;
        } else {
            m_items.erase(it);
        }
        return true;
    }
    return false;
}

const size_t Storage::size() const {
    return m_items.size();
}

}   // namespace items
}   // namespace engine

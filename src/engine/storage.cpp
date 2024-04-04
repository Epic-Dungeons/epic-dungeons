#include "storage.h"
#include "logging/logger.h"

namespace engine {
namespace items {

std::shared_ptr<Item> Storage::getItem(const std::string &id) const {
    for (const auto &item : *this) {
        if (item->id == id) {
            return item;
        }
    }
    return nullptr;
}

bool Storage::addItem(const std::shared_ptr<Item> &item, const size_t &count) {
    if (m_max_items != 0 && m_items.size() == m_max_items) {
        logging::warn("Storage is full");
        return false;
    }
    if (item->isStackable) {
        ItemPtr m_item = getItem(item->id);
        if (m_item) {
            logging::info("Adding item to stack: " + item->id);
            m_items.at(m_item->objectId) += count;
        } else {
            m_items[item->objectId] = count;
            m_set_items.insert(item);
        }
    } else if (count != 1) {
        logging::warn("Item(" + item->id + ") is not stackable, count(" + std::to_string(count) + ") will be ignored");
        m_set_items.insert(item);
        m_items.insert({item->objectId, 1});
    } else {
        m_set_items.insert(item);
        m_items.insert({item->objectId, 1});
    }
    return true;
}

std::vector<std::shared_ptr<Item>> Storage::getItems() const {
    return {m_set_items.begin(), m_set_items.end()};
}

std::vector<std::shared_ptr<Item>> Storage::getItems(const std::string &id) const {
    std::vector<std::shared_ptr<Item>> items;
    for (const auto &item : *this) {
        if (item->id == id) {
            items.push_back(item);
        }
    }
    return items;
}

bool Storage::containsItem(const std::string &id) const {
    for (const auto &item : *this) {
        if (item->id == id) {
            return true;
        }
    }
    return false;
}

bool Storage::containsItem(const std::shared_ptr<Item> &item) const {
    return m_set_items.find(item) != m_set_items.end();
}

bool Storage::removeItem(const std::shared_ptr<Item> &item) {
    logging::debug("Removing item: " + item->id);
    auto it = m_set_items.find(item);
    if (it != m_set_items.end()) {
        m_set_items.erase(it);
        m_items.erase(m_items.find(item->objectId));
        return true;
    }
    return false;
}

bool Storage::removeOneItem(const std::shared_ptr<Item> &item) {
    logging::debug("Removing one item: " + item->id);
    if (!containsItem(item))
        return false;
    if (m_items[item->objectId] > 1) {
        m_items[item->objectId]--;
    } else {
        return removeItem(item);
    }
    return true;
}

const size_t Storage::size() const {
    return m_items.size();
}

const size_t Storage::countItem(const std::string &id) const {
    for (const auto &item : *this) {
        if (item->id == id) {
            return m_items.at(item->objectId);
        }
    }
    return 0;
}

const size_t Storage::countItem(const std::shared_ptr<Item> &item) const {
    if (containsItem(item)) {
        return m_items.at(item->objectId);
    }
    return 0;
}

}   // namespace items
}   // namespace engine


#pragma once
#include "item.h"
#include <map>
#include <memory>
#include <set>

namespace engine {
namespace items {

struct compareSetItems {
    const bool operator()(const std::shared_ptr<const Item> &a, const std::shared_ptr<const Item> &b) const {
        return a->objectId < b->objectId;
    }
};

class Storage {
public:
    using iterator = std::multiset<std::shared_ptr<Item>>::iterator;
    using const_iterator = std::multiset<std::shared_ptr<Item>>::const_iterator;

    iterator begin() {
        return m_items.begin();
    }

    const_iterator begin() const {
        return m_items.begin();
    }

    const_iterator cbegin() const {
        return m_items.cbegin();
    }

    iterator end() {
        return m_items.end();
    }

    const_iterator end() const {
        return m_items.end();
    }

    const_iterator cend() const {
        return m_items.cend();
    }

    bool addItem(const std::shared_ptr<Item> &item);
    std::shared_ptr<Item> getItem(const std::string &id) const;
    std::vector<std::shared_ptr<Item>> getItems() const;
    std::vector<std::shared_ptr<Item>> getItems(const std::string &id) const;
    std::shared_ptr<Item> getItem(const size_t &index) const;
    bool containsItem(const std::shared_ptr<Item> &item) const;
    const size_t size() const;

    bool removeItem(const std::shared_ptr<Item> &item);
    bool removeOneItem(const std::shared_ptr<Item> &item);

    template<typename T>
    std::shared_ptr<T> getItem() const {
        for (auto &item : m_items) {
            if (std::dynamic_pointer_cast<T>(item) != nullptr) {
                return std::dynamic_pointer_cast<T>(item);
            }
        }
        return nullptr;
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> getItems() const {
        std::vector<std::shared_ptr<T>> items;
        for (auto &item : m_items) {
            auto casted = std::dynamic_pointer_cast<T>(item);
            if (casted != nullptr) {
                items.push_back(casted);
            }
        }
        return items;
    }

protected:
    std::multiset<std::shared_ptr<Item>, compareSetItems> m_items;   // <id, item>
    size_t m_max_items = 0;
};

}   // namespace items
}   // namespace engine
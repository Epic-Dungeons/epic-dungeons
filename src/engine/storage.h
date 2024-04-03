
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
    using ItemMap = std::map<uint32_t, size_t>;
    using ItemSet = std::set<std::shared_ptr<Item>, compareSetItems>;
    using iterator = ItemSet::iterator;
    using const_iterator = ItemSet::const_iterator;

    iterator begin() {
        return iterator(m_set_items.begin());
    }

    iterator end() {
        return iterator(m_set_items.end());
    }

    const_iterator begin() const {
        return const_iterator(m_set_items.cbegin());
    }

    const_iterator cbegin() const {
        return const_iterator(m_set_items.cbegin());
    }

    const_iterator end() const {
        return const_iterator(m_set_items.end());
    }

    const_iterator cend() const {
        return const_iterator(m_set_items.cend());
    }

    bool addItem(const std::shared_ptr<Item> &item, const size_t &count = 1);
    std::shared_ptr<Item> getItem(const std::string &id) const;
    std::vector<std::shared_ptr<Item>> getItems() const;
    std::vector<std::shared_ptr<Item>> getItems(const std::string &id) const;
    std::shared_ptr<Item> getItem(const size_t &index) const;
    bool containsItem(const std::shared_ptr<Item> &item) const;

    const size_t countItem(const std::string &id) const;
    const size_t countItem(const std::shared_ptr<Item> &item) const;
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
    ItemMap m_items;   // <id, item>
    ItemSet m_set_items;
    size_t m_max_items = 0;
};

}   // namespace items
}   // namespace engine
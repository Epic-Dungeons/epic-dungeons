#include "storage.h"
#include "logging/logger.h"
#include <string>

namespace gui {
namespace views {

namespace {
size_t in_range(size_t value, size_t min, size_t max) {
    size_t result = value;
    if (value < min)
        result = min;
    if (value > max)
        result = max;
    return result;
}
}   // namespace

Storage &Storage::bind(const std::shared_ptr<engine::items::Storage> &storage) {
    m_storage = storage;
    m_selected_item = 0;
    return *this;
}

Storage &Storage::setMaxVisibleItems(const size_t &max_visible_items) {
    m_max_visible_items = max_visible_items;
    return *this;
}

Storage &Storage::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

Storage &Storage::setState(const State &state) {
    m_state = state;
    return *this;
}

Storage &Storage::setSelectedItem(const size_t &selected_item) {
    if (m_storage.expired()) {
        logging::error("Storage is expired");
        return *this;
    }
    if (m_storage.lock()->size() == 0) {
        m_selected_item = 0;
        return *this;
    }
    m_selected_item = in_range(selected_item, 0, m_storage.lock()->size() - 1);
    return *this;
}

Storage &Storage::nextItem() {
    setSelectedItem(m_selected_item + 1);
    return *this;
}

Storage &Storage::previousItem() {
    if (m_selected_item != 0)
        setSelectedItem(m_selected_item - 1);
    return *this;
}

Storage &Storage::updateSelected() {
    if (m_selected_item >= m_storage.lock()->size()) {
        m_selected_item = m_storage.lock()->size() - 1;
    }
    return *this;
}

std::shared_ptr<engine::items::Item> Storage::getSelectedItem() const {
    return m_storage.lock()->getItems()[m_selected_item];
}

Storage &Storage::setName(const std::string &name) {
    m_name = name;
    return *this;
}

void Storage::draw(const graphics::Renderer &renderer) const {
    if (m_storage.expired()) {
        logging::error("Storage is expired");
        return;
    }
    const auto &storage = *m_storage.lock();
    std::vector<std::string> items;
    for (const auto &item : storage) {
        items.push_back("[" + std::to_string(storage.countItem(item)) + "] " + item->name);
    }
    renderer.draw(graphics::Text(m_name).setFont("story").setFontSize(50).setPosition(m_position));

    if (m_max_visible_items > 0) {
        if (m_selected_item >= m_items_offset + m_max_visible_items - 1) {
            size_t max_offset = storage.size() > m_max_visible_items ? storage.size() - m_max_visible_items : 0;
            m_items_offset = in_range(m_selected_item - m_max_visible_items + 2, 0, max_offset);
        } else if (m_selected_item <= m_items_offset) {
            m_items_offset = m_selected_item;
            if (m_items_offset > 0)
                m_items_offset--;
        }
    } else {
        m_items_offset = 0;
    }

    size_t max_item = storage.size();
    if (m_max_visible_items > 0)
        max_item = std::min(m_items_offset + m_max_visible_items, max_item);
    for (size_t i = m_items_offset; i < max_item; i++) {
        auto text = graphics::Text(items[i])
                        .setFontSize(20)
                        .setPosition(m_position + Vector2d {0, 70 + (i - m_items_offset) * 20})
                        .setStyle(graphics::Text::Style::Italic);
        graphics::Color color = {255, 255, 255, 255};
        if (m_state == State::kSelection && i != m_selected_item)
            color = {255, 255, 255, 100};
        text.setFillColor(color);
        renderer.draw(text);
    }
}

Storage &Storage::takeSelectedItem(const std::shared_ptr<engine::items::Storage> &storage) {
    if (m_storage.expired()) {
        logging::error("Storage is expired");
        return *this;
    }
    if (m_storage.lock()->size() == 0)
        return *this;
    if (m_selected_item >= m_storage.lock()->size()) {
        logging::error("Selected item is out of range");
        return *this;
    }
    auto item = m_storage.lock()->getItems()[m_selected_item];
    storage->addItem(item, m_storage.lock()->countItem(item));
    m_storage.lock()->removeItem(item);
    updateSelected();
    return *this;
}

Storage &Storage::takeOneSelectedItem(const std::shared_ptr<engine::items::Storage> &storage) {
    if (m_storage.expired()) {
        logging::error("Storage is expired");
        return *this;
    }
    if (m_storage.lock()->size() == 0)
        return *this;
    if (m_selected_item >= m_storage.lock()->size()) {
        logging::error("Selected item is out of range");
        return *this;
    }
    auto item = m_storage.lock()->getItems()[m_selected_item];
    storage->addItem(item);
    m_storage.lock()->removeOneItem(item);
    updateSelected();
    return *this;
}

std::shared_ptr<engine::items::Storage> Storage::getStorage() const {
    return m_storage.lock();
}

}   // namespace views
}   // namespace gui
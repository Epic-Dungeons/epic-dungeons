#include "item_info.h"
#include "logging/logger.h"
#include <stdexcept>

namespace gui {
namespace views {

namespace {
std::string to_signed_string(const int &value) {
    if (value >= 0)
        return "+" + std::to_string(value);
    return std::to_string(value);
}
}   // namespace

std::string change_in_parentheses(const int &value, const int &change, bool is_percent = false) {
    if (is_percent)
        return "(" + std::to_string(value) + "%->" + std::to_string(value + change) + "%)";
    else
        return "(" + std::to_string(value) + "->" + std::to_string(value + change) + ")";
}   // namespace

ItemInfo &ItemInfo::bind(const std::shared_ptr<engine::items::Item> &item) {
    m_item = item;
    return *this;
}

ItemInfo &ItemInfo::bindEntity(const std::shared_ptr<engine::entities::Entity> &entity) {
    if (entity == nullptr)
        m_entity.reset();
    else
        m_entity = entity;
    return *this;
}

ItemInfo &ItemInfo::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

void ItemInfo::draw(const graphics::Renderer &renderer) const {
    if (m_item.expired())
        return;
    const auto &item = m_item.lock();
    if (item == nullptr)
        return;

    std::string stats = "";
    auto consumable = std::dynamic_pointer_cast<engine::items::Consumable>(item);
    if (consumable != nullptr) {
        stats += "Consumable\n";
        if (consumable->healthMod != 0) {
            stats += "Health: " + to_signed_string(consumable->healthMod);
            if (!m_entity.expired()) {
                const auto &entity = m_entity.lock();
                stats += " " + change_in_parentheses(entity->getHealth(), consumable->healthMod);
            }
        }

        if (consumable->speedMod != 0) {
            stats += "Speed: " + to_signed_string(consumable->speedMod);
            if (!m_entity.expired()) {
                const auto &entity = m_entity.lock();
                stats += " " + change_in_parentheses(entity->getSpeed(), consumable->speedMod);
            }
        }
        if (consumable->attackMod != 0) {
            stats += "Attack: " + to_signed_string(consumable->attackMod) + "%";
            if (!m_entity.expired()) {
                const auto &entity = m_entity.lock();
                stats += " " + change_in_parentheses(entity->getAttack(), consumable->attackMod, true);
            }
        }

        if (consumable->defenseMod != 0) {
            stats += "Dodge: " + to_signed_string(consumable->defenseMod) + "%";
            if (!m_entity.expired()) {
                const auto &entity = m_entity.lock();
                stats += " " + change_in_parentheses(entity->getDodge(), consumable->defenseMod, true);
            }
        }
        if (consumable->protectionMod != 0) {
            stats += "Protection: " + to_signed_string(consumable->protectionMod) + "%";
            if (!m_entity.expired()) {
                const auto &entity = m_entity.lock();
                stats += " " + change_in_parentheses(entity->getProtection(), consumable->protectionMod, true);
            }
        }
        if (consumable->duration != 0)
            stats += "Duration: " + std::to_string(consumable->duration) + " turns\n";
    }
    auto weapon = std::dynamic_pointer_cast<engine::items::Weapon>(item);
    if (weapon != nullptr) {
        stats += "Weapon\n";
        stats += "Damage: " + std::to_string(weapon->minDamage) + "-" + std::to_string(weapon->maxDamage) + "\n";
        stats += "Speed: " + std::to_string(weapon->speedMod) + "\n";
        stats += "Crit: " + std::to_string(weapon->criticalChanceMod) + "%\n";
    }
    auto armor = std::dynamic_pointer_cast<engine::items::Armor>(item);
    if (armor != nullptr) {
        stats += "Armor\n";
        stats += "Protection: " + std::to_string(armor->protectionMod) + "%\n";
        stats += "Dodge: " + std::to_string(armor->defenseMod) + "%\n";
    }

    renderer.draw(graphics::Text(item->name).setFont("story").setFontSize(50).setPosition(m_position));
    renderer.draw(graphics::Text(item->description).setFontSize(20).setPosition(m_position + Vector2d {0, 50}));
    renderer.draw(graphics::Text(stats).setFontSize(20).setPosition(m_position + Vector2d {0, 80}));
}

}   // namespace views
}   // namespace gui
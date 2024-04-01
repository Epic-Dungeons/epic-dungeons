#pragma once
#include "dungeon/dungeon.h"
#include "engine/consumables/heal.h"
#include "engine/consumables/poison.h"
#include "engine/entities.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "gui_controller/views/skill.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <cmath>
#include <memory>

namespace gui {
namespace game {
class PartyMenu : public GameState {
    enum class PartyMenuState {
        kMain,
        kInventory,
        kSkills,
    };

    std::shared_ptr<engine::entities::Party> getParty() {
        return m_engine.lock()->getParty();
    }

    void enter(GameMachine *gm) override {
        m_engine = gm->m_engine;
        m_selected_hero = 0;
        m_selected_skill = 0;
        km.reset();
        for (auto &hero : getParty()->getMembers()) {
            views::Entity::getView(hero)->setSelection(views::Entity::Selection::kSelectable).setDrawStats(false);
        }
        views::Entity::getView(getParty()->getMember(m_selected_hero))
            ->setSelection(views::Entity::Selection::kSelected)
            .setDrawStats(true);
    }

    void updateMain(GameMachine *gm) {
        if (km.isClicked(keyboard::MOVE_LEFT) || km.isClicked(keyboard::MOVE_RIGHT)) {
            views::Entity::getView(getParty()->getMember(m_selected_hero))
                ->setSelection(views::Entity::Selection::kSelectable)
                .setDrawStats(false);
            if (km.isClicked(keyboard::MOVE_RIGHT)) {
                m_selected_hero = (m_selected_hero + getParty()->getMembersCount() - 1) % getParty()->getMembersCount();
            } else {
                m_selected_hero = (m_selected_hero + 1) % getParty()->getMembersCount();
            }
            views::Entity::getView(getParty()->getMember(m_selected_hero))
                ->setSelection(views::Entity::Selection::kSelected)
                .setDrawStats(true);
        }
        if (km.isClicked(keyboard::KEY_I)) {
            m_state = PartyMenuState::kInventory;
        }
        if (km.isClicked(keyboard::KEY_O)) {
            m_state = PartyMenuState::kSkills;
        }
    }

    void updateInventory(GameMachine *gm) {
        const auto &inventory = getParty()->getMember(m_selected_hero)->getInventory();
        if (km.isClicked(keyboard::MOVE_UP) || km.isClicked(keyboard::MOVE_DOWN)) {
            if (km.isClicked(keyboard::MOVE_UP)) {
                m_selected_item = (m_selected_item + inventory->size() - 1) % inventory->size();
            } else {
                m_selected_item = (m_selected_item + 1) % inventory->size();
            }
        }
        if (km.isClicked(keyboard::KEY_ENTER)) {
            auto item = inventory->getItems()[m_selected_item];
            auto consumable = std::dynamic_pointer_cast<engine::items::Consumable>(item);
            if (consumable != nullptr) {
                consumable->use(getParty()->getMember(m_selected_hero));
                inventory->removeOneItem(item);
            }
            m_selected_item %= inventory->size();
        }

        if (km.isClicked(keyboard::KEY_H)) {
            inventory->addItem(std::make_shared<engine::items::Heal>());
            logging::info("Adding heal to inventory");
        }
        if (km.isClicked(keyboard::KEY_P)) {
            inventory->addItem(std::make_shared<engine::items::Poison>());
            logging::info("Adding poison to inventory");
        }

        if (km.isClicked(keyboard::KEY_I)) {
            m_state = PartyMenuState::kMain;
        }
    }

    void updateSkills(GameMachine *gm) {
        auto skills = getParty()->getMember(m_selected_hero)->getSkills();
        if (km.isClicked(keyboard::MOVE_LEFT) || km.isClicked(keyboard::MOVE_RIGHT)) {
            if (km.isClicked(keyboard::MOVE_LEFT)) {
                m_selected_skill = (m_selected_skill + skills.size() - 1) % skills.size();
            } else if (km.isClicked(keyboard::MOVE_RIGHT)) {
                m_selected_skill = (m_selected_skill + 1) % skills.size();
            }
        }
        if (km.isClicked(keyboard::KEY_O)) {
            m_state = PartyMenuState::kMain;
        }
    }

    void update(GameMachine *gm) override {
        km.update();

        if (km.isClicked(keyboard::KEY_TAB)) {
            gm->changeState(gm->m_previous_state_id);   // return to previous state
            return;
        }
        if (m_state == PartyMenuState::kMain) {
            updateMain(gm);
        } else if (m_state == PartyMenuState::kInventory) {
            updateInventory(gm);
        } else if (m_state == PartyMenuState::kSkills) {
            updateSkills(gm);
        }

        render(gm->m_renderer.lock());
    }

    void exit(GameMachine *gm) override {
        for (auto &hero : getParty()->getMembers()) {
            views::Entity::getView(hero)->setSelection(views::Entity::Selection::kNone).setDrawStats(false);
        }
    }

    std::string to_signed_string(int32_t value) {
        if (value > 0)
            return "+" + std::to_string(value);
        return std::to_string(value);
    }

    void drawInventory(const std::shared_ptr<graphics::Renderer> &r) {
        auto party = getParty();
        auto hero = party->getMember(m_selected_hero);
        const auto &inventory = *hero->getInventory();
        std::vector<std::string> items;
        Vector2d base = {cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 18};
        for (const auto &item : inventory) {
            items.push_back("[" + std::to_string(item->stackSize) + "] " + item->name);
        }

        r->draw(graphics::Text("Inventory").setFont("story").setFontSize(50).setPosition(base));
        for (uint8_t i = 0; i < items.size(); i++) {
            auto text = graphics::Text(items[i])
                            .setFontSize(20)
                            .setPosition({base.x, base.y + 70 + i * 20})
                            .setStyle(graphics::Text::Style::Italic);
            if (m_state == PartyMenuState::kInventory) {
                if (i == m_selected_item) {
                    text.setFillColor({255, 255, 255, 255});
                } else {
                    text.setFillColor({255, 255, 255, 100});
                }
            }
            r->draw(text);
        }

        if (m_state == PartyMenuState::kInventory) {
            Vector2d base = {cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 2};
            auto item = inventory.getItems()[m_selected_item];
            std::string stats = "";
            auto consumable = std::dynamic_pointer_cast<engine::items::Consumable>(item);
            if (consumable != nullptr) {
                stats += "Consumable\n";
                if (consumable->healthMod != 0)
                    stats += "Health: " + to_signed_string(consumable->healthMod) + " ("
                        + std::to_string(hero->getHealth()) + "->"
                        + std::to_string(
                                 std::max(std::min(hero->getHealth() + consumable->healthMod, hero->getMaxHealth()), 0))
                        + ")\n";
                if (consumable->speedMod != 0)
                    stats += "Speed: " + std::to_string(consumable->speedMod) + " (" + std::to_string(hero->getSpeed())
                        + "->" + std::to_string(hero->getSpeed() + consumable->speedMod) + ")\n";
                if (consumable->attackMod != 0)
                    stats += "Attack: " + to_signed_string(consumable->attackMod) + "% ("
                        + std::to_string(hero->getAttack()) + "%->"
                        + std::to_string(hero->getAttack() + consumable->attackMod) + "%)\n";
                if (consumable->defenseMod != 0)
                    stats += "Dodge: " + to_signed_string(consumable->defenseMod) + "% ("
                        + std::to_string(hero->getDodge()) + "%->"
                        + std::to_string(hero->getDodge() + consumable->defenseMod) + "%)\n";
                if (consumable->protectionMod != 0)
                    stats += "Protection: " + to_signed_string(consumable->protectionMod) + "% ("
                        + std::to_string(hero->getProtection()) + "%->"
                        + std::to_string(hero->getProtection() + consumable->protectionMod) + "%)\n";
                if (consumable->duration != 0)
                    stats += "Duration: " + std::to_string(consumable->duration) + " turns\n";
            }
            auto weapon = std::dynamic_pointer_cast<engine::items::Weapon>(item);
            if (weapon != nullptr) {
                stats += "Weapon\n";
                stats +=
                    "Damage: " + std::to_string(weapon->minDamage) + "-" + std::to_string(weapon->maxDamage) + "\n";
                stats += "Speed: " + std::to_string(weapon->speedMod) + "\n";
                stats += "Crit: " + std::to_string(weapon->criticalChanceMod) + "%\n";
            }
            auto armor = std::dynamic_pointer_cast<engine::items::Armor>(item);
            if (armor != nullptr) {
                stats += "Armor\n";
                stats += "Protection: " + std::to_string(armor->protectionMod) + "%\n";
                stats += "Dodge: " + std::to_string(armor->defenseMod) + "%\n";
            }

            r->draw(graphics::Text(item->name).setFont("story").setFontSize(50).setPosition(base));
            r->draw(graphics::Text(item->description).setFontSize(20).setPosition(base + Vector2d {0, 50}));
            r->draw(graphics::Text(stats).setFontSize(20).setPosition(base + Vector2d {0, 80}));
        }
    }

    Vector2d getSkillPosition(const size_t &i, const size_t &w = 3, const float &offset = 100.0f) {
        const float x = offset * (i % w);
        const float y = offset * (i / w);
        return {x, y};
    }

    void drawSkills(const std::shared_ptr<graphics::Renderer> &r) {
        auto party = getParty();
        auto hero = party->getMember(m_selected_hero);
        auto skills = hero->getSkills();
        Vector2d base = {cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 2};
        r->draw(graphics::Text("Skills").setFont("story").setFontSize(50).setPosition(base));

        for (uint8_t i = 0; i < skills.size(); i++) {
            auto skill = skills[i];
            auto view = views::Skill::getView(skill);
            view->setPosition(base + getSkillPosition(i, 3, view->getSize().x * 1.2) + Vector2d {0, 70});
            if (m_state == PartyMenuState::kSkills) {
                if (i == m_selected_skill) {
                    view->setSelection(views::Skill::Selection::kSelected);
                } else {
                    view->setSelection(views::Skill::Selection::kSelectable);
                }
            }
            r->draw(view);
        }

        if (m_state == PartyMenuState::kSkills) {
            auto skill = skills[m_selected_skill];
            Vector2d base = {cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 18};
            r->draw(graphics::Text(skill->name).setFont("story").setFontSize(50).setPosition(base));
            std::string description = "";
            auto combat = std::dynamic_pointer_cast<engine::skills::CombatSkill>(skill);
            if (combat != nullptr) {
                auto weapon = hero->getWeapon();
                description += "Combat Skill\n";
                int32_t min_damage = round(weapon->minDamage * (100 + combat->damageMod) / 100);
                int32_t max_damage = round(weapon->maxDamage * (100 + combat->damageMod) / 100);
                int32_t crit_chance = hero->getCrit() + combat->criticalChanceMod;
                int32_t attack = hero->getAttack() + combat->attackMod;
                std::string launchable_positions = "";
                std::string targetable_positions = "";
                for (auto &pos : combat->launchablePositions)
                    launchable_positions += std::to_string(pos) + " ";
                for (auto &pos : combat->targetablePositions)
                    targetable_positions += std::to_string(pos) + " ";
                description += "Damage: " + std::to_string(min_damage) + "-" + std::to_string(max_damage) + "\n";
                description += "Crit: " + std::to_string(crit_chance) + "%\n";
                description += "Attack: " + std::to_string(attack) + "%\n";
                description += "Launch: " + launchable_positions + "\n";
                description += "Target: " + targetable_positions + "\n";
            }
            r->draw(graphics::Text(description).setFontSize(20).setPosition(base + Vector2d {0, 70}));
        }
    }

    void render(const std::shared_ptr<graphics::Renderer> &r) {
        r->clear();
        auto party = getParty();
        for (uint8_t i = 0; i < party->getMembersCount(); i++) {
            auto hero = party->getMember(i);
            r->draw(views::Entity::getView(hero)->setPosition(utils::getEntityPosition(3 - i)));
        }
        r->draw(graphics::Text("Party Menu")
                    .setFontSize(50)
                    .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18}));
        if (m_state == PartyMenuState::kMain) {
            r->draw(graphics::Text("Press I to open inventory")
                        .setFontSize(20)
                        .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18 + 70}));
            r->draw(graphics::Text("Press O to open skills")
                        .setFontSize(20)
                        .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18 + 100}));
            drawInventory(r);
            drawSkills(r);
        }
        if (m_state == PartyMenuState::kInventory) {
            r->draw(graphics::Text("Press I to close inventory")
                        .setFontSize(20)
                        .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18 + 70}));
            drawInventory(r);
        }
        if (m_state == PartyMenuState::kSkills) {
            r->draw(graphics::Text("Press O to close skills")
                        .setFontSize(20)
                        .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18 + 70}));
            drawSkills(r);
        }
        r->display();
    }

    KeyboardManager km;
    std::weak_ptr<engine::Engine> m_engine;
    uint8_t m_selected_hero;
    uint8_t m_selected_skill;
    uint32_t m_selected_item;
    PartyMenuState m_state;
};

}   // namespace game
}   // namespace gui
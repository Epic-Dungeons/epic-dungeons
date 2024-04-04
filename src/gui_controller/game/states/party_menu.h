#pragma once
#include "dungeon/dungeon.h"
#include "engine/entities.h"
#include "engine/items/consumables/bandage.h"
#include "engine/items/consumables/poison.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "gui_controller/views/item_info.h"
#include "gui_controller/views/skill.h"
#include "gui_controller/views/skill_info.h"
#include "gui_controller/views/skill_selection.h"
#include "gui_controller/views/storage.h"
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
        kTeammateChoice,
    };

    std::shared_ptr<engine::entities::Party> getParty() {
        return m_engine.lock()->getParty();
    }

    void enter(GameMachine *gm) override {
        m_engine = gm->m_engine;
        m_selected_hero = 0;
        m_selected_skill = 0;
        m_selected_teammate = 0;
        km.reset();
        for (auto &hero : getParty()->getMembers()) {
            views::Entity::getView(hero)->setSelection(views::Entity::Selection::kSelectable).setDrawStats(false);
        }
        views::Entity::getView(getParty()->getMember(m_selected_hero))
            ->setSelection(views::Entity::Selection::kSelected)
            .setDrawStats(true);
        v_storage.bind(getParty()->getInventory()).setMaxVisibleItems(5);
        v_storage.setPosition({cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 18});
        v_skill_selection.setPosition({cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 2});
        v_skill_selection.bind(getParty()->getMember(m_selected_hero)->getSkills());
        v_skill_info.setPosition({cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 18});
        v_skill_info.bind(v_skill_selection.getSelectedSkill());
        v_skill_info.bindEntity(getParty()->getMember(m_selected_hero));
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
            v_storage.bind(getParty()->getInventory());
            v_skill_selection.bind(getParty()->getMember(m_selected_hero)->getSkills());
            v_skill_info.bindEntity(getParty()->getMember(m_selected_hero));
        }
        if (km.isClicked(keyboard::KEY_I)) {
            m_state = PartyMenuState::kInventory;
            v_storage.setState(views::Storage::State::kSelection);
        }
        if (km.isClicked(keyboard::KEY_O)) {
            m_state = PartyMenuState::kSkills;
            v_skill_selection.setState(views::SkillSelection::State::kSelection);
            v_skill_info.bind(v_skill_selection.getSelectedSkill());
        }
        if (km.isClicked(keyboard::KEY_C)) {
            const auto &teammates = m_engine.lock()->getCampTeammates();
            m_selected_teammate = std::find(teammates.begin(), teammates.end(), getParty()->getMember(m_selected_hero))
                - teammates.begin();
            m_state = PartyMenuState::kTeammateChoice;
        }
    }

    void updateInventory(GameMachine *gm) {
        const auto &inventory = getParty()->getInventory();
        if (km.isClicked(keyboard::MOVE_UP)) {
            v_storage.previousItem();
        } else if (km.isClicked(keyboard::MOVE_DOWN)) {
            v_storage.nextItem();
        }
        if (km.isClicked(keyboard::KEY_ENTER)) {
            auto item = v_storage.getSelectedItem();
            auto consumable = std::dynamic_pointer_cast<engine::items::Consumable>(item);
            if (consumable != nullptr) {
                consumable->use(getParty()->getMember(m_selected_hero));
                inventory->removeOneItem(item);
                v_storage.updateSelected();
            }
        }

        if (km.isClicked(keyboard::KEY_H)) {
            inventory->addItem(std::make_shared<engine::items::Bandage>());
            logging::info("Adding heal to inventory");
        }
        if (km.isClicked(keyboard::KEY_P)) {
            inventory->addItem(std::make_shared<engine::items::Poison>());
            logging::info("Adding poison to inventory");
        }
        if (km.isClicked(keyboard::KEY_R)) {
            inventory->addItem(std::make_shared<engine::items::Item>("item", "Item", "An item", 0));
            logging::info("Adding poison to inventory");
        }

        if (km.isClicked(keyboard::KEY_I)) {
            m_state = PartyMenuState::kMain;
            v_storage.setState(views::Storage::State::kNone);
            v_skill_selection.bind(getParty()->getMember(m_selected_hero)->getSkills());   // update skills
        }
    }

    void updateSkills(GameMachine *gm) {
        if (km.isClicked(keyboard::MOVE_LEFT)) {
            v_skill_selection.previousSkill();
            v_skill_info.bind(v_skill_selection.getSelectedSkill());
        } else if (km.isClicked(keyboard::MOVE_RIGHT)) {
            v_skill_selection.nextSkill();
            v_skill_info.bind(v_skill_selection.getSelectedSkill());
        }
        if (km.isClicked(keyboard::KEY_O)) {
            m_state = PartyMenuState::kMain;
            v_skill_selection.setState(views::SkillSelection::State::kNone);
        }
    }

    void updateTeammateChoice(GameMachine *gm) {
        const auto &teammates = m_engine.lock()->getCampTeammates();

        if (km.isClicked(keyboard::MOVE_LEFT) || km.isClicked(keyboard::MOVE_RIGHT)) {
            views::Entity::getView(teammates[m_selected_teammate])
                ->setSelection(views::Entity::Selection::kSelectable)
                .setDrawStats(false);
            if (km.isClicked(keyboard::MOVE_LEFT)) {
                m_selected_teammate = (m_selected_teammate + teammates.size() - 1) % teammates.size();
            } else if (km.isClicked(keyboard::MOVE_RIGHT)) {
                m_selected_teammate = (m_selected_teammate + 1) % teammates.size();
            }
            views::Entity::getView(teammates[m_selected_teammate])
                ->setSelection(views::Entity::Selection::kSelected)
                .setDrawStats(true);
        }
        if (km.isClicked(keyboard::KEY_ENTER)) {
            auto hero = getParty()->getMember(m_selected_hero);
            auto teammate = teammates[m_selected_teammate];
            getParty()->swapMembers(hero, teammate);
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
        } else if (m_state == PartyMenuState::kTeammateChoice) {
            updateTeammateChoice(gm);
        }

        render(gm->m_renderer.lock());
    }

    void exit(GameMachine *gm) override {
        for (auto &hero : getParty()->getMembers()) {
            views::Entity::getView(hero)->setSelection(views::Entity::Selection::kNone).setDrawStats(false);
        }
    }

    void drawInventory(const std::shared_ptr<graphics::Renderer> &r) {
        r->draw(v_storage);
        if (m_state == PartyMenuState::kInventory) {
            auto item = v_storage.getSelectedItem();
            if (item != nullptr) {
                v_item_info.bind(item)
                    .bindEntity(getParty()->getMember(m_selected_hero))
                    .setPosition({cfg::WINDOW_WIDTH * 2 / 3, cfg::WINDOW_HEIGHT / 2});
                r->draw(v_item_info);
            }
        }
    }

    void drawSkills(const std::shared_ptr<graphics::Renderer> &r) {
        r->draw(v_skill_selection);
        if (m_state == PartyMenuState::kSkills)
            r->draw(v_skill_info);
    }

    void drawTeammateChoice(const std::shared_ptr<graphics::Renderer> &r) {
        auto party = getParty();
        auto hero = party->getMember(m_selected_hero);
        const auto &teammates = m_engine.lock()->getCampTeammates();
        Vector2d base = utils::getEntityPosition(1);
        Vector2d offset = utils::getEntityPosition(1) - utils::getEntityPosition(0);
        for (uint8_t i = 0; i < teammates.size(); i++) {
            auto teammate = teammates[i];
            auto view = views::Entity::getView(teammate);
            view->setPosition(base + offset * i);
            if (m_state == PartyMenuState::kTeammateChoice) {
                if (i == m_selected_teammate) {
                    view->setSelection(views::Entity::Selection::kSelected);
                } else {
                    view->setSelection(views::Entity::Selection::kSelectable);
                }
            }
            r->draw(view);
        }
    }

    void render(const std::shared_ptr<graphics::Renderer> &r) {
        r->clear();
        auto party = getParty();
        if (m_state != PartyMenuState::kTeammateChoice) {
            for (uint8_t i = 0; i < party->getMembersCount(); i++) {
                auto hero = party->getMember(i);
                r->draw(views::Entity::getView(hero)->setPosition(utils::getEntityPosition(3 - i)));
            }
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
            r->draw(graphics::Text("Press C to change member")
                        .setFontSize(20)
                        .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18 + 130}));
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
        if (m_state == PartyMenuState::kTeammateChoice) {
            r->draw(graphics::Text("Press Enter to select")
                        .setFontSize(20)
                        .setPosition({cfg::WINDOW_WIDTH / 20, cfg::WINDOW_HEIGHT / 18 + 70}));
            drawTeammateChoice(r);
        }
        r->display();
    }

    KeyboardManager km;
    std::weak_ptr<engine::Engine> m_engine;
    uint8_t m_selected_hero;
    uint8_t m_selected_skill;
    uint32_t m_selected_teammate;
    PartyMenuState m_state;

    views::Storage v_storage;
    views::ItemInfo v_item_info;
    views::SkillSelection v_skill_selection;
    views::SkillInfo v_skill_info;
};

}   // namespace game
}   // namespace gui
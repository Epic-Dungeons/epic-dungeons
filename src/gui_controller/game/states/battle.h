#pragma once
#include "dungeon/dungeon.h"
#include "engine/entity.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/utils.h"
#include "keyboard/keyboard.h"
#include "static_data/game_config.h"
#include <gui_controller/keyboard_manager/keyboard_manager.h>
#include <memory>

namespace gui {
namespace game {
class Battle : public GameState {
public:
    enum class BattleState {
        kAttackerSelection,
        kSkillSelection,
        kDefenderSelection,
        kAttack,
    };

    Battle() : GameState() {
        m_enemy_party = std::make_shared<engine::entities::Party>();
        m_enemy_party->addMember(std::make_shared<engine::entities::Highwayman>());
        m_enemy_party->addMember(std::make_shared<engine::entities::Highwayman>());
        m_enemy_party->addMember(std::make_shared<engine::entities::Highwayman>());
    }

    virtual void enter(GameMachine *gm) {
        m_keyboard.reset();
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
        m_skills.resize(gm->m_engine.lock()->getParty()->getMembersCount());
        for (int i = 0; i < gm->m_engine.lock()->getParty()->getMembersCount(); i++) {
            auto member = gm->m_engine.lock()->getParty()->getMember(i);
            for (auto &skill : member->getSkills()) {
                m_skills[i].push_back(skill);
            }
        }
        m_selected = 0;
        m_state = BattleState::kAttackerSelection;
        render(gm);
    }

    void attacker_selection(GameMachine *gm) {
        if (m_keyboard.isClicked(keyboard::KEY_ENTER)) {
            m_attacker = gm->m_engine.lock()->getParty()->getMember(m_selected);
            m_state = BattleState::kSkillSelection;
        }
        if (m_keyboard.isClicked(keyboard::KEY_RIGHT)) {
            m_selected = (m_selected - 1 + gm->m_engine.lock()->getParty()->getMembersCount()) % gm->m_engine.lock()->getParty()->getMembersCount();
        }
        if (m_keyboard.isClicked(keyboard::KEY_LEFT)) {
            m_selected = (m_selected + 1) % gm->m_engine.lock()->getParty()->getMembersCount();
        }
        render(gm);
    }

    void skill_selection(GameMachine *gm) {
        if (m_keyboard.isClicked(keyboard::KEY_ENTER)) {
            m_skill = std::dynamic_pointer_cast<engine::skills::CombatSkill>(m_skills[m_selected][m_selected_skill]);
            m_state = BattleState::kDefenderSelection;
        }
        if (m_keyboard.isClicked(keyboard::KEY_RIGHT)) {
            m_selected_skill = (m_selected_skill - 1 + m_skills.size()) % m_skills.size();
        }
        if (m_keyboard.isClicked(keyboard::KEY_LEFT)) {
            m_selected_skill = (m_selected_skill + 1) % m_skills.size();
        }
        render(gm);
    }

    void defender_selection(GameMachine *gm) {
        if (m_keyboard.isClicked(keyboard::KEY_ENTER)) {
            m_defenders.push_back(m_enemy_party->getMember(m_selected_defender));
            m_state = BattleState::kAttack;
        }
        if (m_keyboard.isClicked(keyboard::KEY_RIGHT)) {
            m_selected_defender = (m_selected_defender + 1) % m_enemy_party->getMembersCount();
        }
        if (m_keyboard.isClicked(keyboard::KEY_LEFT)) {
            m_selected_defender = (m_selected_defender - 1 + m_enemy_party->getMembersCount()) % m_enemy_party->getMembersCount();
        }
        render(gm);
    }

    virtual void update(GameMachine *gm) {
        m_keyboard.update();
        switch (m_state) {
            case BattleState::kAttackerSelection:
                attacker_selection(gm);
                break;
            case BattleState::kSkillSelection:
                skill_selection(gm);
                break;
            case BattleState::kDefenderSelection:
                defender_selection(gm);
                break;
            case BattleState::kAttack:
                attack(gm);
                break;
        }
    }

    void attack(GameMachine *gm) {
        auto attacker = m_attacker.lock();
        auto skill = m_skill.lock();
        if (attacker && skill) {
            for (auto &defender : m_defenders) {
                if (defender.lock()) {
                    defender.lock()->takeAttack(attacker, skill);
                }
            }
        }
    }

    void drawSkill(std::shared_ptr<graphics::Renderer> r, std::shared_ptr<engine::skills::Skill> skill) {
        if (cached_skills.find(skill->id) == cached_skills.end()) {
            cached_skills[skill->id] = std::make_shared<graphics::Sprite>("skills/" + skill->id + ".png");
        }
        // todo: draw skill
    }

    void render(GameMachine *gm) {
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
        std::shared_ptr<dungeon::Cell> cell = gm->m_engine.lock()->getDungeon()->getCurrentCell().lock();
        std::shared_ptr<dungeon::Dungeon> dungeon = gm->m_engine.lock()->getDungeon();
        std::shared_ptr<engine::entities::Party> party = gm->m_engine.lock()->getParty();
        r->clear();
        utils::cellView(r, dungeon);
        
        if (m_state == BattleState::kAttackerSelection) {
            for (size_t i = 0; i < party->getMembersCount(); i++)
                utils::drawEntity(r, party->getMember(i), 3 - i, m_selected == i);
        } else {
            for (size_t i = 0; i < party->getMembersCount(); i++)
                utils::drawEntity(r, party->getMember(i), 3 - i);
        }

        if (m_state == BattleState::kSkillSelection) {
            for (size_t i = 0; i < m_skills.size(); i++) {
                drawSkill(r, m_skills[m_selected][i]);
            }
        }

        if (m_state == BattleState::kDefenderSelection) {
            for (size_t i = 0; i < m_enemy_party->getMembersCount(); i++)
                utils::drawEntity(r, m_enemy_party->getMember(i), 4 + i, m_selected_defender == i);
        } else {
            for (size_t i = 0; i < m_enemy_party->getMembersCount(); i++)
                utils::drawEntity(r, m_enemy_party->getMember(i), 4 + i);
        }
        r->display();
    }

private:
    std::shared_ptr<engine::entities::Party> m_enemy_party;
    KeyboardManager m_keyboard;
    uint8_t m_selected = 0;
    uint8_t m_selected_skill = 0;
    u_int8_t m_selected_defender = 0;

    std::weak_ptr<engine::entities::Entity> m_attacker;
    std::vector<std::weak_ptr<engine::entities::Entity>> m_defenders;
    std::weak_ptr<engine::skills::CombatSkill> m_skill;

    std::vector<std::vector<std::shared_ptr<engine::skills::Skill>>> m_skills;
    std::map<std::string, std::shared_ptr<graphics::Sprite>> cached_skills;

    BattleState m_state = BattleState::kAttackerSelection;
};

}   // namespace game
}   // namespace gui
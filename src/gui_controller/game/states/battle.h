#pragma once
#include "dungeon/dungeon.h"
#include "engine/entities.h"
#include "engine/skill.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/hero.h"
#include "gui_controller/views/skill_selection.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include "vector2d/vector2d.h"
#include <cmath>
#include <memory>

namespace gui {
namespace game {
class Battle : public GameState {
    using EntityPtr = std::shared_ptr<engine::entities::Entity>;
    enum class State {
        kStart,
        kPlayerTurn,
        kEnemyTurn,
        kAttack,
        kNextEntity,
        kEnd,
    };

    static std::vector<std::shared_ptr<engine::entities::Entity>> getEnemies(GameMachine *gm) {
        static std::map<dungeon::Position, std::vector<std::shared_ptr<engine::entities::Entity>>> enemies;
        auto cell_pos = gm->m_engine.lock()->getDungeon()->getCurrentCell().lock()->getPosition();
        if (enemies.find(cell_pos) == enemies.end())
            enemies[cell_pos] = engine::entities::createEnemies();
        return enemies[cell_pos];
    }

    void freeze(GameMachine *gm) override {}

    void unfreeze(GameMachine *gm) override {}

    uint8_t getPosition(const EntityPtr &entity) {
        if (std::find(m_enemies.begin(), m_enemies.end(), entity) == m_enemies.end())
            return 1 + entity->getPosition();
        else
            return 1 + std::distance(m_enemies.begin(), std::find(m_enemies.begin(), m_enemies.end(), entity));
        return 0;
    }

    std::vector<std::shared_ptr<engine::entities::Entity>> getPossibleTargets(const views::Entity &entity,
                                                                              const engine::skills::Skill &skill) {
        std::vector<std::shared_ptr<engine::entities::Entity>> result;
        views::Entity::Direction target_team = views::Entity::Direction::kRight;
        views::Entity::Direction entity_team = entity.getDirection();
        logging::info("Entity team: {}", entity_team == views::Entity::Direction::kRight ? "Right" : "Left");

        if (skill.targetType == engine::skills::TargetType::kSelf) {
            logging::info("Target team: Self");
            if (skill.isUsable(getPosition(entity.getEntity())))
                return {entity.getEntity()};
            return {};
        }

        if (skill.targetType == engine::skills::TargetType::kIndividual
            || skill.targetType == engine::skills::TargetType::kOtherParty) {
            target_team = entity_team == views::Entity::Direction::kRight ? views::Entity::Direction::kLeft
                                                                          : views::Entity::Direction::kRight;
        } else {   // kParty or kSelf
            target_team = entity_team;
        }
        logging::info("Target team: {}", target_team == views::Entity::Direction::kRight ? "Right" : "Left");
        for (const auto &target : m_queue) {
            if (views::Entity::getView(target)->getDirection() == target_team && skill.isTargetable(getPosition(target))
                && target->isAlive())
                result.push_back(target);
        }

        return result;
    }

    void enter(GameMachine *gm) override {
        m_state = State::kStart;
        km.reset();
        m_enemies = getEnemies(gm);
        m_queue = gm->m_engine.lock()->getParty()->getMembers();
        for (const auto &enemy : m_enemies)
            m_queue.push_back(enemy);

        std::sort(m_queue.begin(), m_queue.end(), [](const EntityPtr &a, const EntityPtr &b) {
            return a->getSpeed() > b->getSpeed();   // descending order
        });

        for (const auto &entity : m_queue) {
            views::Entity::getView(entity)->setState(views::Entity::State::kCombat);
        }
        m_current_entity = 0;
        auto party = gm->m_engine.lock()->getParty();
        for (const auto &entity : party->getMembers()) {
            views::Entity::getView(entity)
                ->setPosition(utils::getEntityPosition(3 - entity->getPosition()))
                .setSelection(views::Entity::Selection::kSelectable)
                .setDirection(views::Entity::Direction::kRight);
        }
        uint8_t position = 0;
        for (const auto &entity : m_enemies) {
            views::Entity::getView(entity)
                ->setPosition(utils::getEntityPosition(position++ + 4))
                .setSelection(views::Entity::Selection::kSelectable)
                .setDirection(views::Entity::Direction::kLeft);
        }
        views::Entity::getView(m_queue[m_current_entity])->setSelection(views::Entity::Selection::kSelected);
        m_skill_selection.setOrigin(views::SkillSelection::Origin::kTopCenter)
            .setPosition({cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT / 18})
            .setState(views::SkillSelection::State::kSelection);
    }

    void enemyTurn(GameMachine *gm) {
        auto enemy = m_queue[m_current_entity];
        logging::info("Enemy {} turn, Team: {}", enemy->getNameWithId(),
                      views::Entity::getView(enemy)->getDirection() == views::Entity::Direction::kRight ? "Right"
                                                                                                        : "Left");
        logging::info("Enemy {} skills: {}", enemy->getNameWithId(), enemy->getSkills().size());
        auto skill = enemy->getSkills()[rand() % enemy->getSkills().size()];
        auto targets = getPossibleTargets(*views::Entity::getView(enemy), *skill);
        if (targets.empty())
            return;
        m_targets.clear();
        m_current_skill = skill;
        if (skill->targetType == engine::skills::TargetType::kIndividual
            && skill->type == engine::skills::Type::kMelee) {
            auto target = targets[rand() % targets.size()];
            m_targets.push_back(target);
        } else {
            for (const auto &target : targets) {
                m_targets.push_back(target);
            }
        }
        m_state = State::kAttack;
    }

    bool selecting_skill = true;
    int32_t selected_enemy = 0;

    void playerTurn(GameMachine *gm) {
        auto entity = m_queue[m_current_entity];
        if (selecting_skill) {
            if (km.isClicked(keyboard::Hotkey::MOVE_RIGHT)) {
                m_skill_selection.nextSkill();
            }
            if (km.isClicked(keyboard::Hotkey::MOVE_LEFT)) {
                m_skill_selection.previousSkill();
            }
            if (km.isClicked(keyboard::KEY_ENTER)) {
                m_current_skill = m_skill_selection.getSelectedSkill();
                m_targets.clear();
                if (m_current_skill->targetType == engine::skills::TargetType::kSelf) {
                    m_targets.push_back(entity);
                    m_state = State::kAttack;
                } else {
                    m_targets = getPossibleTargets(*views::Entity::getView(entity), *m_current_skill);
                    if (m_targets.empty()) {
                        m_current_skill = nullptr;
                        logging::info("No targets");
                        return;
                    }
                    selected_enemy = 0;
                    if (m_current_skill->targetType == engine::skills::TargetType::kIndividual
                        && m_current_skill->type == engine::skills::Type::kMelee) {
                        views::Entity::getView(m_targets[selected_enemy])
                            ->setSelection(views::Entity::Selection::kSelected)
                            .setChance(entity->calculateHitChance(m_targets[selected_enemy], m_current_skill))
                            .setDrawChance(true);
                    } else {
                        for (const auto &target : m_targets) {
                            views::Entity::getView(target)
                                ->setSelection(views::Entity::Selection::kSelected)
                                .setChance(entity->calculateHitChance(target, m_current_skill))
                                .setDrawChance(true);
                        }
                    }
                    selecting_skill = false;
                }
            }
        } else {
            if (m_current_skill->targetType == engine::skills::TargetType::kIndividual) {
                if (km.isClicked(keyboard::Hotkey::MOVE_RIGHT)) {
                    views::Entity::getView(m_targets[selected_enemy])
                        ->setSelection(views::Entity::Selection::kSelectable)
                        .setDrawChance(false);
                    selected_enemy++;
                    if (selected_enemy >= m_targets.size())
                        selected_enemy = 0;
                    views::Entity::getView(m_targets[selected_enemy])
                        ->setSelection(views::Entity::Selection::kSelected)
                        .setChance(entity->calculateHitChance(m_targets[selected_enemy], m_current_skill))
                        .setDrawChance(true);
                }
                if (km.isClicked(keyboard::Hotkey::MOVE_LEFT)) {
                    views::Entity::getView(m_targets[selected_enemy])
                        ->setSelection(views::Entity::Selection::kSelectable)
                        .setDrawChance(false);
                    selected_enemy--;
                    if (selected_enemy < 0)
                        selected_enemy = m_targets.size() - 1;
                    views::Entity::getView(m_targets[selected_enemy])
                        ->setSelection(views::Entity::Selection::kSelected)
                        .setChance(entity->calculateHitChance(m_targets[selected_enemy], m_current_skill))
                        .setDrawChance(true);
                }
                if (km.isClicked(keyboard::Hotkey::MOVE_DOWN)) {
                    for (const auto &target : m_targets) {
                        views::Entity::getView(target)
                            ->setSelection(views::Entity::Selection::kSelectable)
                            .setDrawChance(false);
                    }
                    m_targets.clear();
                    m_state = State::kPlayerTurn;
                    selecting_skill = true;
                }
                if (km.isClicked(keyboard::KEY_ENTER)) {
                    auto target = m_targets[selected_enemy];
                    m_targets.clear();
                    m_targets.push_back(target);
                    m_state = State::kAttack;
                    selecting_skill = true;
                }
            } else {
                for (const auto &target : m_targets) {
                    views::Entity::getView(target)
                        ->setSelection(views::Entity::Selection::kSelected)
                        .setChance(entity->calculateHitChance(target, m_current_skill))
                        .setDrawChance(true);
                }
                if (km.isClicked(keyboard::KEY_ENTER)) {
                    m_state = State::kAttack;
                    selecting_skill = true;
                }
            }
        }

        // m_state = State::kAttack;
    }

    void attack(GameMachine *gm) {
        auto attacker = m_queue[m_current_entity];
        auto skill = m_current_skill;
        auto combat_skill = std::dynamic_pointer_cast<engine::skills::CombatSkill>(skill);
        auto consume_skill = std::dynamic_pointer_cast<engine::skills::ConsumeItemSkill>(skill);

        if (combat_skill != nullptr) {
            for (const auto &target : m_targets)
                target->takeAttack(attacker, combat_skill);
        }
        if (consume_skill != nullptr) {
            consume_skill->consumable->use(attacker);
            consume_skill->storage->removeOneItem(consume_skill->consumable);
        }
        m_current_skill = nullptr;
        m_targets.clear();
        m_state = State::kNextEntity;
    }

    void check_enemeies_dead() {
        std::vector<std::shared_ptr<engine::entities::Entity>> dead_enemies;
        for (const auto &enemy : m_enemies) {
            if (!enemy->isAlive())
                dead_enemies.push_back(enemy);
        }
        for (const auto &enemy : dead_enemies) {
            m_enemies.erase(std::find(m_enemies.begin(), m_enemies.end(), enemy));
            m_queue.erase(std::find(m_queue.begin(), m_queue.end(), enemy));
        }
        for (const auto &enemy : m_enemies) {
            views::Entity::getView(enemy)->setPosition(utils::getEntityPosition(
                4 + std::distance(m_enemies.begin(), std::find(m_enemies.begin(), m_enemies.end(), enemy))));
        }
    }

    bool check_teams_dead(GameMachine *gm) {
        if (m_enemies.size() == 0) {
            m_state = State::kEnd;
            return true;
        }
        bool is_enemy_alive = std::any_of(m_enemies.begin(), m_enemies.end(), [](const EntityPtr &enemy) {
            return enemy->isAlive();
        });
        auto party = gm->m_engine.lock()->getParty()->getMembers();
        bool is_party_alive = std::any_of(party.begin(), party.end(), [](const EntityPtr &entity) {
            return entity->isAlive();
        });
        if (!is_party_alive || !is_enemy_alive) {
            m_state = State::kEnd;
            return true;
        }
        return false;
    }

    void bind_selector() {
        m_skill_selection.bind(m_queue[m_current_entity]->getSkills());
        logging::info("Binding selector: position: {}", getPosition(m_queue[m_current_entity]));
        for (const auto &skill : m_skill_selection.getSkills()) {
            if (!skill->getSkill()->isUsable(getPosition(m_queue[m_current_entity]))
                || getPossibleTargets(*views::Entity::getView(m_queue[m_current_entity]), *skill->getSkill()).empty())
                skill->setSelection(views::Skill::Selection::kNotSelectable);
        }
        logging::info("Binding selector: done");
    }

    void update(GameMachine *gm) override {
        km.update();
        render(gm->m_renderer.lock(), gm->m_engine.lock());

        if (m_state == State::kEnd) {
            logging::info("End");
            gm->changeState(GUIGameState::kCellMovement);
            return;
        }
        if (m_state == State::kStart) {
            logging::info("Start");
            if (check_teams_dead(gm))
                return;
            views::Entity::getView(m_queue[m_current_entity])->setSelection(views::Entity::Selection::kSelectable);
            while (!m_queue[m_current_entity]->isAlive()) {
                m_current_entity++;
                if (m_current_entity >= m_queue.size())
                    m_current_entity = 0;
            }
            bind_selector();
            if (views::Entity::getView(m_queue[m_current_entity])
                    ->setSelection(views::Entity::Selection::kSelected)
                    .getDirection()
                == views::Entity::Direction::kRight) {
                logging::info("Player turn");
                m_state = State::kPlayerTurn;
            } else
                m_state = State::kEnemyTurn;
            if (!m_skill_selection.fixSelection())
                m_state = State::kNextEntity;
            return;
        }
        if (m_state == State::kPlayerTurn) {
            playerTurn(gm);
            return;
        }
        if (m_state == State::kEnemyTurn) {
            logging::info("Enemy turn");
            enemyTurn(gm);
            return;
        }
        if (m_state == State::kAttack) {
            logging::info("Attack");
            attack(gm);
            return;
        }
        if (m_state == State::kNextEntity) {
            logging::info("Next entity");
            check_enemeies_dead();
            for (const auto &entity : m_queue)
                views::Entity::getView(entity)
                    ->setSelection(views::Entity::Selection::kSelectable)
                    .setDrawChance(false);

            if (check_teams_dead(gm))
                return;
            m_current_entity++;
            if (m_current_entity >= m_queue.size())
                m_current_entity = 0;
            while (!m_queue[m_current_entity]->isAlive()) {
                m_current_entity++;
                if (m_current_entity >= m_queue.size())
                    m_current_entity = 0;
            }
            bind_selector();
            if (views::Entity::getView(m_queue[m_current_entity])
                    ->setSelection(views::Entity::Selection::kSelected)
                    .getDirection()
                == views::Entity::Direction::kRight) {
                m_state = State::kPlayerTurn;
                logging::info("Player turn");
            } else
                m_state = State::kEnemyTurn;
            if (!m_skill_selection.fixSelection())
                m_state = State::kNextEntity;
            return;
        }
    }

    void render(std::shared_ptr<graphics::Renderer> r, std::shared_ptr<engine::Engine> e) {
        r->clear();
        std::shared_ptr<dungeon::Dungeon> d = e->getDungeon();
        std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
        std::shared_ptr<engine::entities::Party> party = e->getParty();
        utils::cellView(r, d);
        for (const auto &entity : m_queue) {
            r->draw(views::Entity::getView(entity));
        }
        if (m_state == State::kPlayerTurn)
            r->draw(m_skill_selection);

        r->display();
    }

    void exit(GameMachine *gm) override {
        m_enemies.clear();
        m_queue.clear();
        m_targets.clear();
        for (const auto &entity : gm->m_engine.lock()->getParty()->getMembers()) {
            views::Entity::getView(entity)->setState(views::Entity::State::kIdle);
        }
        m_current_skill = nullptr;
    }

private:
    KeyboardManager km;
    State m_state;
    std::vector<std::shared_ptr<engine::entities::Entity>> m_enemies;
    std::vector<std::shared_ptr<engine::entities::Entity>> m_queue;

    views::SkillSelection m_skill_selection;

    std::vector<std::shared_ptr<engine::entities::Entity>> m_targets;
    std::shared_ptr<engine::skills::Skill> m_current_skill;
    int32_t m_current_entity = 0;
};

}   // namespace game
}   // namespace gui
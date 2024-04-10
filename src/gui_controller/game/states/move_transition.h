#pragma once
#include "dungeon/dungeon.h"
#include "gui_controller/game/game_machine.h"
#include "gui_controller/keyboard_manager/keyboard_manager.h"
#include "gui_controller/timed_count.h"
#include "gui_controller/utils.h"
#include "gui_controller/views/gradient.h"
#include "gui_controller/views/hero.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include "vector2d/vector2d.h"
#include <cmath>
#include <map>
#include <memory>

namespace gui {
namespace game {
class MoveTransition : public GameState {
public:
    MoveTransition() {
        auto sigm = [](float x) {
            return 1 / (1 + std::exp(-10 * (x - 0.5)));
        };
        m_anim.init(0, 1, 500, sigm);
        v_map = views::Map::getView();
    }

    virtual void enter(GameMachine *gm) {
        m_anim.start();
        sound::playSound("walking-sound", 80);
        for (const auto &entity : gm->m_engine.lock()->getParty()->getMembers()) {
            views::Entity::getView(entity)->setState("walk");
        }
        v_map->bind(gm->m_engine.lock()->getDungeon());
        v_map->startAnimation();
    }

    virtual void update(GameMachine *gm) {
        std::shared_ptr<graphics::Renderer> r = gm->m_renderer.lock();
        std::shared_ptr<dungeon::Dungeon> d = gm->m_engine.lock().get()->getDungeon();
        m_anim.update(gm->getDeltaTime());
        if (m_anim.isEnded() && v_map->isAnimationEnded()) {
            gm->changeState(GUIGameState::kEvent);
        }
        for (const auto &entity : gm->m_engine.lock()->getCampTeammates()) {
            views::Entity::getView(entity)->update(gm->getDeltaTime());
        }
        render(r, gm->m_engine.lock());
    }

    void render(std::shared_ptr<graphics::Renderer> r, std::shared_ptr<engine::Engine> e) {
        std::shared_ptr<dungeon::Dungeon> d = e->getDungeon();
        std::shared_ptr<dungeon::Cell> current = d->getCurrentCell().lock();
        std::shared_ptr<dungeon::Cell> next_cell = d->getNextCell().lock();
        std::shared_ptr<engine::entities::Party> party = e->getParty();
        float animation_progress = m_anim.get();

        int direction = 1;
        if (next_cell && next_cell != d->getNextOnPath().lock()) {
            direction = -1;
        }

        r->clear();
        utils::cellView(r, d, animation_progress);
        for (size_t i = 0; i < party->getMembersCount(); i++) {
            utils::drawEntity(r, party->getMember(i), 3 - i, false, animation_progress * direction);
        }
        if (d->getCurrentCell().lock()->isRoom() || d->getNextCell().lock()->isRoom()) {
            uint8_t alpha = 255 * animation_progress;
            r->draw(graphics::Rectangle(0, 0, cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, {0, 0, 0, alpha}));
        }
        Vector2d center = {cfg::WINDOW_WIDTH * 4 / 5, cfg::WINDOW_HEIGHT / 2};
        r->draw(views::Gradient::getView());
        r->draw(v_map->setPosition(center));
        r->display();
    }

    void exit(GameMachine *gm) {
        gm->m_engine.lock()->getDungeon()->setCurrentCell(gm->m_engine.lock()->getDungeon()->getNextCell().lock());
        for (const auto &entity : gm->m_engine.lock()->getParty()->getMembers()) {
            views::Entity::getView(entity)->setState("idle");
        }
    }

private:
    TimedCount m_anim;
    std::shared_ptr<views::Map> v_map;
};
}   // namespace game
}   // namespace gui

#pragma once
#include "engine/entity.h"
#include "graphics/graphics.h"
#include "gui_controller/timed_count.h"
#include "gui_controller/view.h"
#include "vector2d/vector2d.h"
#include <memory>
#include <string>

namespace gui {
namespace views {

class DamageAnimation : public View {
public:
    DamageAnimation();
    DamageAnimation &setDuration(uint64_t duration);
    DamageAnimation &start();
    DamageAnimation &setPosition(const Vector2d &position);
    DamageAnimation &setDamage(const int &damage);
    DamageAnimation &update();

    const bool isRunning() const;

protected:
    void draw(const graphics::Renderer &renderer) const override;

private:
    TimedCount m_timer;
    int32_t m_damage = 0;
    Vector2d m_position = {0, 0};
};

class Entity : public View {
public:
    Entity(const std::shared_ptr<engine::entities::Entity> &entity) : View() {
        bind(entity);
        reset();
    }

    enum class State {
        kIdle,
        kCombat,
        kWalking,
    };

    enum class Selection {
        kNone,
        kSelected,
        kSelectable,
        kNotSelectable,
    };

    enum class Direction {
        kLeft,
        kRight,
    };

    const std::shared_ptr<engine::entities::Entity> getEntity() const {
        return m_entity.lock();
    }

    static const std::shared_ptr<Entity> getView(const std::shared_ptr<engine::entities::Entity> &entity);

    void reset();

    Entity &setState(const State &state);
    Entity &setPosition(const Vector2d &position);
    Entity &setSelection(const Selection &selection);
    Entity &setDrawStats(bool draw_stats);
    Entity &setDirection(const Direction &direction);
    Entity &setChance(const float &chance);
    Entity &setDrawChance(const bool &draw_chance);

    Direction getDirection() const;
    Vector2d getPosition() const;
    void applyDamage(const int &damage) const;

    const graphics::SpritePtr &getPortrait() const;

protected:
    void bind(const std::shared_ptr<engine::entities::Entity> &entity);
    void draw(const graphics::Renderer &renderer) const override;
    void drawStats(const graphics::Renderer &renderer, const Vector2d &position) const;

    std::weak_ptr<engine::entities::Entity> m_entity;
    std::map<State, std::shared_ptr<graphics::Animation>> m_animations;
    std::shared_ptr<graphics::Sprite> m_portrait;
    std::shared_ptr<graphics::Sprite> m_grave;
    State m_state = State::kIdle;
    Selection m_selection;
    Vector2d m_position = {0, 0};

private:
    bool m_draw_stats = false;
    mutable DamageAnimation m_damage_anim;
    mutable int32_t m_prev_hp = 0;
    Direction m_direction = Direction::kRight;
    float m_chance = 0.0f;
    bool m_draw_chance = false;
};

}   // namespace views
}   // namespace gui
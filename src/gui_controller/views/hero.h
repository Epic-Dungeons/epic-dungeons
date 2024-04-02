#pragma once
#include "engine/entity.h"
#include "graphics/graphics.h"
#include "gui_controller/view.h"
#include "vector2d/vector2d.h"
#include <memory>
#include <string>

namespace gui {
namespace views {

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

    const std::shared_ptr<engine::entities::Entity> getEntity() const {
        return m_entity.lock();
    }

    static const std::shared_ptr<Entity> getView(const std::shared_ptr<engine::entities::Entity> &entity);

    void reset();

    Entity &setState(const State &state);
    Entity &setPosition(const Vector2d &position);
    Entity &setSelection(const Selection &selection);
    Entity &setDrawStats(bool draw_stats);

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
};

}   // namespace views
}   // namespace gui
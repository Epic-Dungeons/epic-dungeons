#pragma once
#include "engine/game_object.h"
#include "graphics/graphics.h"
#include <map>
#include <memory>

namespace gui {
namespace views {

class View : public graphics::Drawable, public std::enable_shared_from_this<View> {
public:
    View() = default;
    virtual ~View() = default;

protected:
    virtual void draw(const graphics::Renderer &renderer) const = 0;
};

namespace {
static std::map<size_t, std::shared_ptr<View>> m_views;
}   // namespace

class ViewManager {
public:
    ViewManager() = delete;

    static void bind(const engine::GameObject &object, const std::shared_ptr<View> &view) {
        m_views[object.objectId] = view;
    }

    static void unbind(const engine::GameObject &object) {
        m_views.erase(object.objectId);
    }

    static std::shared_ptr<View> getView(const engine::GameObject &object) {
        if (m_views.find(object.objectId) != m_views.end()) {
            return m_views[object.objectId];
        }
        return nullptr;
    }
};

}   // namespace views
}   // namespace gui

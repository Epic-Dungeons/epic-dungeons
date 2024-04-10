#pragma once

#include "graphics/color.h"
#include "graphics/drawables/drawable.h"
#include "graphics/renderer.h"
#include "graphics/spine/extension.h"
#include "graphics/spine/spine.h"
#include "logging/logger.h"
#include "vector2d/vector2d.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace graphics {

typedef std::function<void(spAnimationState *state, int trackIndex, spEventType type, spEvent *event, int loopCount)>
    AnimationCallback;

class Spine : public Drawable {
public:
    Spine(const std::string &skeletonPath, const std::string &atlasPath, float scale = 1);

    Spine &setPosition(const Vector2d &position);
    Spine &setFlip(bool flipX, bool flipY);
    Spine &setTimeScale(float timeScale);
    Spine &setColor(const Color &color);
    Spine &setLoop(bool loop);
    Spine &start();
    Spine &stop();
    Spine &reset();
    bool isRunning() const;
    bool isLooping() const;

    Spine &setAnimation(const std::string &animation);
    bool hasAnimation(const std::string &animation) const;

    void update(uint32_t deltaTime);

    Vector2d getPosition() const;
    Vector2d getSize() const;
    std::vector<std::string> getAnimations() const;

    static std::shared_ptr<Spine> load(const std::string &skeletonPath, const std::string &atlasPath, float scale = 1);

protected:
    void draw(const Renderer &renderer) const override;
    virtual spAnimationStateListener getCallback();
    Color m_color = Color::White;

private:
    bool m_is_running = true;
    bool isLoop = false;
    float timeScale = 1;
    std::unique_ptr<spSkeleton, decltype(&spSkeleton_dispose)> m_skeleton;
    std::unique_ptr<spAnimationState, decltype(&spAnimationState_dispose)> m_state;
    std::unique_ptr<spAnimationStateData, decltype(&spAnimationStateData_dispose)> m_stateData;
    std::unique_ptr<spSkeletonData, decltype(&spSkeletonData_dispose)> m_skeletonData;
    std::unique_ptr<spAtlas, decltype(&spAtlas_dispose)> m_atlas;
    std::unique_ptr<spSkeletonBounds, decltype(&spSkeletonBounds_dispose)> m_bounds;
    std::unique_ptr<sf::VertexArray> m_vertexArray;
    std::unique_ptr<float[]> m_worldVertices;
};
}   // namespace graphics
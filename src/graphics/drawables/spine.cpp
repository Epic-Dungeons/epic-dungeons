#include "graphics/drawables/spine.h"

#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
    sf::Texture *texture = new sf::Texture();
    if (!texture->loadFromFile(path))
        return;
    texture->setSmooth(true);
    self->rendererObject = texture;
    sf::Vector2u size = texture->getSize();
    self->width = size.x;
    self->height = size.y;
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    delete (sf::Texture *) self->rendererObject;
}

char *_spUtil_readFile(const char *path, int *length) {
    return _readFile(path, length);
}

namespace graphics {

Spine::Spine(const std::string &skeletonPath, const std::string &atlasPath, float scale) :
    // clang-format off
    m_skeleton(nullptr, spSkeleton_dispose),
    m_state(nullptr, spAnimationState_dispose),
    m_stateData(nullptr, spAnimationStateData_dispose),
    m_skeletonData(nullptr, spSkeletonData_dispose),
    m_atlas(nullptr, spAtlas_dispose),
    m_bounds(nullptr, spSkeletonBounds_dispose),
    m_vertexArray(nullptr),
    m_worldVertices(nullptr)
// clang-format on
{
    m_atlas = std::unique_ptr<spAtlas, decltype(&spAtlas_dispose)>(spAtlas_createFromFile(atlasPath.c_str(), 0),
                                                                   spAtlas_dispose);
    if (!m_atlas) {
        LOG_ERROR("Error reading atlas file: %s\n", atlasPath.c_str());
        exit(0);
    }
    spSkeletonJson *json = spSkeletonJson_create(m_atlas.get());
    json->scale = scale;   // TODO: make this configurable

    m_skeletonData = std::unique_ptr<spSkeletonData, decltype(&spSkeletonData_dispose)>(
        spSkeletonJson_readSkeletonDataFile(json, skeletonPath.c_str()), spSkeletonData_dispose);
    if (!m_skeletonData) {
        LOG_ERROR("Error reading skeleton data file: %s\n", json->error);
        exit(0);
    }
    spSkeletonJson_dispose(json);
    m_bounds = std::unique_ptr<spSkeletonBounds, decltype(&spSkeletonBounds_dispose)>(spSkeletonBounds_create(),
                                                                                      spSkeletonBounds_dispose);
    // Configure mixing.
    m_stateData = std::unique_ptr<spAnimationStateData, decltype(&spAnimationStateData_dispose)>(
        spAnimationStateData_create(m_skeletonData.get()), spAnimationStateData_dispose);

    spBone_setYDown(true);
    m_worldVertices = std::unique_ptr<float[]>(new float[SPINE_MESH_VERTEX_COUNT_MAX]);
    m_skeleton = std::unique_ptr<spSkeleton, decltype(&spSkeleton_dispose)>(spSkeleton_create(m_skeletonData.get()),
                                                                            spSkeleton_dispose);

    m_stateData = std::unique_ptr<spAnimationStateData, decltype(&spAnimationStateData_dispose)>(
        spAnimationStateData_create(m_skeletonData.get()), spAnimationStateData_dispose);

    m_state = std::unique_ptr<spAnimationState, decltype(&spAnimationState_dispose)>(
        spAnimationState_create(m_stateData.get()), spAnimationState_dispose);

    m_vertexArray =
        std::unique_ptr<sf::VertexArray>(new sf::VertexArray(sf::Triangles, m_skeletonData->bonesCount * 4));

    spSkeleton_setToSetupPose(m_skeleton.get());
    spSkeleton_updateWorldTransform(m_skeleton.get());

    m_state->spine = this;
    m_state->listener = getCallback();
}

void Spine::update(uint32_t deltaTime) {
    // if (!m_is_running)
    //     return;
    float delta = (float) deltaTime / 1000.0f;
    spSkeleton_update(m_skeleton.get(), delta);
    spAnimationState_update(m_state.get(), delta);
    spAnimationState_apply(m_state.get(), m_skeleton.get());
    spSkeleton_updateWorldTransform(m_skeleton.get());
}

Spine &Spine::setPosition(const Vector2d &position) {
    m_skeleton->x = position.x;
    m_skeleton->y = position.y;
    spSkeleton_updateWorldTransform(m_skeleton.get());
    return *this;
}

Spine &Spine::setFlip(bool flipX, bool flipY) {
    m_skeleton->flipX = flipX;
    m_skeleton->flipY = flipY;
    return *this;
}

Spine &Spine::setTimeScale(float timeScale) {
    m_state->timeScale = timeScale;
    return *this;
}

bool Spine::hasAnimation(const std::string &animation) const {
    return spSkeletonData_findAnimation(m_skeleton->data, animation.c_str()) != nullptr;
}

Spine &Spine::setAnimation(const std::string &animation) {
    spAnimation *anim = spSkeletonData_findAnimation(m_skeleton->data, animation.c_str());
    if (!anim) {
        LOG_ERROR("Animation %s not found\n", animation.c_str());
        exit(0);
    }
    spAnimationState_setAnimationByName(m_state.get(), 0, animation.c_str(), true);
    return *this;
}

Spine &Spine::setColor(const Color &color) {
    m_color = color;
    return *this;
}

Spine &Spine::setLoop(bool loop) {
    isLoop = loop;
    return *this;
}

Spine &Spine::start() {
    m_is_running = true;
    return *this;
}

Spine &Spine::stop() {
    m_is_running = false;
    return *this;
}

bool Spine::isRunning() const {
    return m_is_running;
}

bool Spine::isLooping() const {
    return isLoop;
}

Spine &Spine::reset() {
    spSkeleton_setToSetupPose(m_skeleton.get());
    spAnimationState_clearTracks(m_state.get());
    return *this;
}

Vector2d Spine::getPosition() const {
    return Vector2d(m_skeleton->x, m_skeleton->y);
}

Vector2d Spine::getSize() const {
    spSkeletonBounds_update(m_bounds.get(), m_skeleton.get(), true);
    return Vector2d(m_bounds->maxX - m_bounds->minX, m_bounds->maxY - m_bounds->minY);
}

std::vector<std::string> Spine::getAnimations() const {
    std::vector<std::string> animations;
    for (int i = 0; i < m_skeletonData->animationsCount; ++i) {
        animations.push_back(m_skeletonData->animations[i]->name);
    }
    return animations;
}

void Spine::draw(const Renderer &renderer) const {
    m_vertexArray->clear();
    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;

    sf::Vertex vertices[4];
    sf::Vertex vertex;
    for (int i = 0; i < m_skeleton->slotsCount; ++i) {
        spSlot *slot = m_skeleton->drawOrder[i];
        spAttachment *attachment = slot->attachment;
        if (!attachment)
            continue;
        sf::Texture *texture = 0;
        if (attachment->type == SP_ATTACHMENT_REGION) {
            spRegionAttachment *regionAttachment = (spRegionAttachment *) attachment;
            texture = (sf::Texture *) ((spAtlasRegion *) regionAttachment->rendererObject)->page->rendererObject;
            spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, m_worldVertices.get());

            sf::Uint8 r = static_cast<sf::Uint8>(m_skeleton->r * slot->r * m_color.r);
            sf::Uint8 g = static_cast<sf::Uint8>(m_skeleton->g * slot->g * m_color.g);
            sf::Uint8 b = static_cast<sf::Uint8>(m_skeleton->b * slot->b * m_color.b);
            sf::Uint8 a = static_cast<sf::Uint8>(m_skeleton->a * slot->a * m_color.a);

            sf::Vector2u size = texture->getSize();
            vertices[0].color.r = r;
            vertices[0].color.g = g;
            vertices[0].color.b = b;
            vertices[0].color.a = a;
            vertices[0].position.x = m_worldVertices[SP_VERTEX_X1];
            vertices[0].position.y = m_worldVertices[SP_VERTEX_Y1];
            vertices[0].texCoords.x = regionAttachment->uvs[SP_VERTEX_X1] * size.x;
            vertices[0].texCoords.y = regionAttachment->uvs[SP_VERTEX_Y1] * size.y;

            vertices[1].color.r = r;
            vertices[1].color.g = g;
            vertices[1].color.b = b;
            vertices[1].color.a = a;
            vertices[1].position.x = m_worldVertices[SP_VERTEX_X2];
            vertices[1].position.y = m_worldVertices[SP_VERTEX_Y2];
            vertices[1].texCoords.x = regionAttachment->uvs[SP_VERTEX_X2] * size.x;
            vertices[1].texCoords.y = regionAttachment->uvs[SP_VERTEX_Y2] * size.y;

            vertices[2].color.r = r;
            vertices[2].color.g = g;
            vertices[2].color.b = b;
            vertices[2].color.a = a;
            vertices[2].position.x = m_worldVertices[SP_VERTEX_X3];
            vertices[2].position.y = m_worldVertices[SP_VERTEX_Y3];
            vertices[2].texCoords.x = regionAttachment->uvs[SP_VERTEX_X3] * size.x;
            vertices[2].texCoords.y = regionAttachment->uvs[SP_VERTEX_Y3] * size.y;

            vertices[3].color.r = r;
            vertices[3].color.g = g;
            vertices[3].color.b = b;
            vertices[3].color.a = a;
            vertices[3].position.x = m_worldVertices[SP_VERTEX_X4];
            vertices[3].position.y = m_worldVertices[SP_VERTEX_Y4];
            vertices[3].texCoords.x = regionAttachment->uvs[SP_VERTEX_X4] * size.x;
            vertices[3].texCoords.y = regionAttachment->uvs[SP_VERTEX_Y4] * size.y;

            m_vertexArray->append(vertices[0]);
            m_vertexArray->append(vertices[1]);
            m_vertexArray->append(vertices[2]);
            m_vertexArray->append(vertices[0]);
            m_vertexArray->append(vertices[2]);
            m_vertexArray->append(vertices[3]);

        } else if (attachment->type == SP_ATTACHMENT_MESH) {
            spMeshAttachment *mesh = (spMeshAttachment *) attachment;
            if (mesh->verticesCount > SPINE_MESH_VERTEX_COUNT_MAX)
                continue;
            texture = (sf::Texture *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;
            spMeshAttachment_computeWorldVertices(mesh, slot, m_worldVertices.get());

            sf::Uint8 r = static_cast<sf::Uint8>(m_skeleton->r * slot->r * m_color.r);
            sf::Uint8 g = static_cast<sf::Uint8>(m_skeleton->g * slot->g * m_color.g);
            sf::Uint8 b = static_cast<sf::Uint8>(m_skeleton->b * slot->b * m_color.b);
            sf::Uint8 a = static_cast<sf::Uint8>(m_skeleton->a * slot->a * m_color.a);
            vertex.color.r = r;
            vertex.color.g = g;
            vertex.color.b = b;
            vertex.color.a = a;

            sf::Vector2u size = texture->getSize();
            for (int i = 0; i < mesh->trianglesCount; ++i) {
                int index = mesh->triangles[i] << 1;
                vertex.position.x = m_worldVertices[index];
                vertex.position.y = m_worldVertices[index + 1];
                vertex.texCoords.x = mesh->uvs[index] * size.x;
                vertex.texCoords.y = mesh->uvs[index + 1] * size.y;
                m_vertexArray->append(vertex);
            }

        } else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH) {
            spSkinnedMeshAttachment *mesh = (spSkinnedMeshAttachment *) attachment;
            if (mesh->uvsCount > SPINE_MESH_VERTEX_COUNT_MAX)
                continue;
            texture = (sf::Texture *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;
            spSkinnedMeshAttachment_computeWorldVertices(mesh, slot, m_worldVertices.get());

            sf::Uint8 r = static_cast<sf::Uint8>(m_skeleton->r * slot->r * m_color.r);
            sf::Uint8 g = static_cast<sf::Uint8>(m_skeleton->g * slot->g * m_color.g);
            sf::Uint8 b = static_cast<sf::Uint8>(m_skeleton->b * slot->b * m_color.b);
            sf::Uint8 a = static_cast<sf::Uint8>(m_skeleton->a * slot->a * m_color.a);
            vertex.color.r = r;
            vertex.color.g = g;
            vertex.color.b = b;
            vertex.color.a = a;

            sf::Vector2u size = texture->getSize();
            for (int i = 0; i < mesh->trianglesCount; ++i) {
                int index = mesh->triangles[i] << 1;
                vertex.position.x = m_worldVertices[index];
                vertex.position.y = m_worldVertices[index + 1];
                vertex.texCoords.x = mesh->uvs[index] * size.x;
                vertex.texCoords.y = mesh->uvs[index + 1] * size.y;
                m_vertexArray->append(vertex);
            }
        }

        if (texture) {
            // SMFL doesn't handle batching for us, so we'll just force a single texture per skeleton.
            states.texture = texture;

            sf::BlendMode blend = slot->data->additiveBlending ? sf::BlendAdd : sf::BlendAlpha;
            if (states.blendMode != blend) {
                renderer.draw(*m_vertexArray, states);
                m_vertexArray->clear();
                states.blendMode = blend;
            }
        }
    }
    renderer.draw(*m_vertexArray, states);
}

spAnimationStateListener Spine::getCallback() {
    return [](spAnimationState *state, int trackIndex, spEventType type, spEvent *event, int loopCount) {
        if (type == SP_ANIMATION_END) {
            auto spine = static_cast<Spine *>(state->spine);
            if (!spine->isLoop) {
                spine->m_is_running = false;
            }
        }
    };
}

std::shared_ptr<Spine> Spine::load(const std::string &skeletonPath, const std::string &atlasPath, float scale) {
    return std::make_shared<Spine>(skeletonPath, atlasPath, scale);
}

}   // namespace graphics
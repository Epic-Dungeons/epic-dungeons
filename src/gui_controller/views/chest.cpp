#include "chest.h"

namespace gui {
namespace views {

namespace {
static std::map<std::string, std::shared_ptr<graphics::Animation>> animations;
static std::map<std::string, std::shared_ptr<graphics::Sprite>> sprites;

static std::shared_ptr<graphics::Animation> getAnimation(const std::string &path) {
    if (animations.find(path) == animations.end()) {
        animations[path] = std::make_shared<graphics::Animation>(path);
    }
    return animations[path];
}

static std::shared_ptr<graphics::Sprite> getSprite(const std::string &path) {
    if (sprites.find(path) == sprites.end()) {
        sprites[path] = std::make_shared<graphics::Sprite>(path);
    }
    return sprites[path];
}
}   // namespace

Chest::Chest() : View() {
    v_storage = std::make_shared<Storage>();
}

Chest &Chest::bind(const std::shared_ptr<engine::Chest> &chest) {
    if (chest == nullptr) {
        m_chest.reset();
        v_storage->bind(nullptr);
        return *this;
    } else {
        m_chest = chest;
        v_storage->bind(chest->getStorage());
        m_sprite = getSprite("res/sprites/chest.png");
    }
    is_open = false;
    return *this;
}

void Chest::open() {
    if (m_chest.expired())
        return;
    is_open = true;
}

void Chest::close() {
    if (m_chest.expired())
        return;
    is_open = false;
}

const bool Chest::isOpen() const {
    return is_open;
}

void Chest::draw(const graphics::Renderer &renderer) const {
    if (m_chest.expired())
        return;

    auto chest = m_chest.lock();
    if (chest == nullptr)
        return;

    m_sprite->setPosition(m_position).setOrigin(graphics::Sprite::Origin::BOTTOM_CENTER);
    renderer.draw(*m_sprite);

    // if (is_open) {
    //     renderer.draw(v_storage->setState(Storage::State::kSelection)
    //                       .setName("Chest")
    //                       .setPosition({cfg::WINDOW_WIDTH / 2, cfg::WINDOW_HEIGHT / 18}));
    // }
}

Chest &Chest::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

const std::shared_ptr<engine::Chest> Chest::getChest() const {
    return m_chest.lock();
}

Chest &Chest::nextItem() {
    if (m_chest.expired())
        return *this;
    v_storage->nextItem();
    return *this;
}

Chest &Chest::previousItem() {
    if (m_chest.expired())
        return *this;
    v_storage->previousItem();
    return *this;
}

const std::shared_ptr<engine::items::Storage> Chest::getStorage() const {
    return m_chest.lock()->getStorage();
}

const std::shared_ptr<Storage> Chest::getStorageView() const {
    return v_storage;
}

}   // namespace views
}   // namespace gui
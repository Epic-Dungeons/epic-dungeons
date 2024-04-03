#include "hero.h"
#include "logging/logger.h"

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

static std::string toSignedString(const int &value) {
    if (value >= 0)
        return "+" + std::to_string(value);
    return std::to_string(value);
}
}   // namespace

void Entity::bind(const std::shared_ptr<engine::entities::Entity> &entity) {
    m_entity = entity;
    m_animations.clear();
    m_portrait = nullptr;
    m_grave = nullptr;

    if (entity) {
        m_portrait = getSprite("heroes/" + entity->getId() + "/portrait.png");
        m_grave = getSprite("heroes/grave.png");
        m_grave->setScale(0.4f).setOrigin(graphics::Sprite::Origin::BOTTOM_CENTER);
        m_animations[State::kIdle] = getAnimation("heroes/" + entity->getId() + "/idle.gif");
        m_animations[State::kCombat] = getAnimation("heroes/" + entity->getId() + "/combat.gif");
        m_animations[State::kWalking] = getAnimation("heroes/" + entity->getId() + "/walk.gif");
        m_prev_hp = entity->getHealth();
    }
}

void Entity::draw(const graphics::Renderer &renderer) const {
    auto entity = m_entity.lock();
    if (!entity) {
        throw std::runtime_error("Entity is not bound");
        return;
    }
    if (m_prev_hp != entity->getHealth()) {
        applyDamage(m_prev_hp - entity->getHealth());
        m_prev_hp = entity->getHealth();
    }

    if (m_draw_stats)
        drawStats(renderer, {0, 0});

    static std::map<Selection, graphics::Color> selection_colors = {
        {Selection::kNone, 0xffffff},
        {Selection::kSelected, 0xffffff},
        {Selection::kSelectable, 0x666666},
        {Selection::kNotSelectable, 0x222222},
    };

    const graphics::Color &selection_color = selection_colors.find(m_selection)->second;

    graphics::SpritePtr sprite;
    if (!entity->isAlive()) {
        sprite = m_grave;
    } else {
        sprite = m_animations.find(m_state)->second;
    }
    if (sprite) {
        sprite->setOrigin(graphics::Sprite::Origin::BOTTOM_CENTER).setColor(selection_color);
        renderer.draw(sprite->setPosition(m_position));
    } else {
        logging::error("No animation found for state " + std::to_string(static_cast<uint8_t>(m_state)));
        renderer.draw(graphics::Sprite("missing.png").setPosition(m_position));
    }

    if (m_damage_anim.isRunning()) {
        m_damage_anim.update();
        renderer.draw(m_damage_anim.setPosition(m_position));
    }
}

void Entity::drawStats(const graphics::Renderer &renderer, const Vector2d &position) const {
    auto entity = m_entity.lock();
    if (!entity) {
        throw std::runtime_error("Entity is not bound");
        return;
    }

    std::string health = "HP: " + std::to_string(entity->getHealth()) + "/" + std::to_string(entity->getMaxHealth());
    std::string speed = "SPD: " + std::to_string(entity->getSpeed());
    // std::string attack = "ATK: " + std::to_string(entity->getAttack()) + "%";
    std::string defense = "DEF: " + std::to_string(entity->getDodge()) + "%";
    std::string crit = "CRT: " + std::to_string(entity->getCrit()) + "%";
    std::string damage =
        "DMG: " + std::to_string(entity->getMinDamage()) + "-" + std::to_string(entity->getMaxDamage());

    graphics::Text stats(health + "\n" + speed + "\n" +
                         //   attack + "\n" +
                         defense + "\n" + crit + "\n" + damage);

    renderer.draw(stats.setOrigin(graphics::Text::Origin::TOP_CENTER).setPosition(position + m_position));
}

Entity &Entity::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

Entity &Entity::setState(const State &state) {
    m_state = state;
    return *this;
}

Entity &Entity::setSelection(const Selection &selection) {
    m_selection = selection;
    return *this;
}

Entity &Entity::setDrawStats(bool draw_stats) {
    m_draw_stats = draw_stats;
    return *this;
}

const graphics::SpritePtr &Entity::getPortrait() const {
    return m_portrait;
}

void Entity::reset() {
    m_state = State::kIdle;
    m_selection = Selection::kNone;
    m_position = {0, 0};
    m_draw_stats = false;
}

const std::shared_ptr<Entity> Entity::getView(const std::shared_ptr<engine::entities::Entity> &entity) {
    auto view = ViewManager::getView(*entity);
    if (view)
        return std::dynamic_pointer_cast<Entity>(view);
    std::shared_ptr<Entity> new_view = std::make_shared<Entity>(entity);
    ViewManager::bind(*entity, new_view);
    return new_view;
}

void Entity::applyDamage(const int &damage) const {
    m_damage_anim.setDamage(damage).setDuration(500).start();
}

DamageAnimation::DamageAnimation() : View() {}

DamageAnimation &DamageAnimation::setDuration(uint64_t duration) {
    m_timer.init(0, 1, duration);   // 0 to 1 in duration seconds
    return *this;
}

DamageAnimation &DamageAnimation::start() {
    m_timer.start();
    logging::info("Damage animation started: " + std::to_string(isRunning()));
    return *this;
}

DamageAnimation &DamageAnimation::setPosition(const Vector2d &position) {
    m_position = position;
    return *this;
}

DamageAnimation &DamageAnimation::setDamage(const int &damage) {
    m_damage = damage;
    return *this;
}

const bool DamageAnimation::isRunning() const {
    return !m_timer.isEnded();
}

void DamageAnimation::draw(const graphics::Renderer &renderer) const {
    if (m_timer.isEnded())
        return;
    float progress = m_timer.get();
    graphics::Text text(toSignedString(-m_damage));
    if (m_damage < 0)                                           // negative damage = healing
        text.setFillColor({0, 255, 0, 255 * (1 - progress)});   // green color
    if (m_damage > 0)                                           // positive damage = damage
        text.setFillColor({255, 0, 0, 255 * (1 - progress)});   // red color

    text.setOrigin(graphics::Text::Origin::BOTTOM_CENTER)
        .setPosition(m_position + Vector2d(0, -50 * progress))
        .setScale(1 + 2 * progress, 1 + 2 * progress);
    renderer.draw(text);
}

DamageAnimation &DamageAnimation::update() {
    m_timer.update();
    return *this;
}

}   // namespace views
}   // namespace gui

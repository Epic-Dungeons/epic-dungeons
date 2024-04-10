#include "hero.h"
#include "engine/parsing/reader.h"
#include "engine/skill.h"
#include "logging/logger.h"
#include <chrono>

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

static std::unordered_map<std::string, std::shared_ptr<graphics::Spine>> spineCache;

static std::shared_ptr<graphics::Spine> getSpine(const std::string &heroId, const std::string &animationId,
                                                 const std ::uint32_t objectId) {
    std::string path = "res/heroes/" + heroId + "/anim/" + heroId + ".sprite." + animationId;
    std::string key = path + std::to_string(objectId);
    if (spineCache.find(key) != spineCache.end())
        return spineCache[key];
    std::shared_ptr<graphics::Spine> spine = graphics::Spine::load(path + ".json", path + ".atlas");
    LOG_DEBUG("Loaded spine animation: {}", path);
    if (spine && spine->hasAnimation(animationId))
        spine->setAnimation(animationId);

    spineCache[key] = spine;
    return spine;
}
}   // namespace

void Entity::bind(const std::shared_ptr<engine::entities::Entity> &entity) {
    m_entity = entity;
    m_animations.clear();
    m_portrait = nullptr;
    m_grave = nullptr;

    if (entity) {
        m_last_update =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        m_portrait = nullptr;
        m_grave = getSprite("res/sprites/heroes/grave.png");
        m_grave->setScale(0.4f).setOrigin(graphics::Sprite::Origin::BOTTOM_CENTER);

        engine::parsing::FileDataPtr art_data =
            engine::parsing::FileReader::read("res/heroes/" + entity->getId() + "/" + entity->getId() + ".art.darkest");
        m_animations["idle"] = getSpine(entity->getId(), "idle", objectId);
        m_animations["idle"]->setLoop(true);
        m_animations["walk"] = getSpine(entity->getId(), "walk", objectId);
        m_animations["walk"]->setLoop(true);
        m_animations["combat"] = getSpine(entity->getId(), "combat", objectId);
        m_animations["combat"]->setLoop(true);
        m_animations["camp"] = getSpine(entity->getId(), "camp", objectId);
        m_animations["camp"]->setLoop(true);

        for (const auto &row : art_data->getRows("combat_skill")) {
            std::string id = row->getString("anim", 0, "");
            if (!id.empty())
                m_animations[id] = getSpine(entity->getId(), id, objectId);
            else
                LOG_ERROR("Animation id not found: {}", id);
        }
        m_prev_hp = entity->getHealth();
    }
}

void Entity::attack(const std::shared_ptr<engine::skills::Skill> &skill,
                    const std::vector<std::shared_ptr<engine::entities::Entity>> &targets) {
    auto entity = m_entity.lock();
    if (!entity) {
        logging::error("Entity::attack: Entity is not bound");
        throw std::runtime_error("Entity::attack: Entity is not bound");
    }
    engine::parsing::FileDataPtr art_data =
        engine::parsing::FileReader::read("res/heroes/" + entity->getId() + "/" + entity->getId() + ".art.darkest");
    std::string skill_id = skill->id;
    skill_id = skill_id.substr(skill_id.find_last_of('/') + 1);   // remove owner from skill id
    engine::parsing::DataRowPtr skill_art =
        art_data->findRows("combat_skill", engine::parsing::StringParam("id", skill_id)).front();
    if (!skill_art) {
        LOG_ERROR("Skill art not found: {}", skill_id);
        throw std::runtime_error("Skill art not found: " + skill_id);
    }
    std::string animation = skill_art->getString("anim", 0, "none");
    if (m_animations.find(animation) != m_animations.end()) {
        m_animations[animation]->setLoop(false);   // setup animation
        setState(animation);
        if (m_animations[animation]->hasAnimation(animation)) {
            is_attacking = true;
        }
    } else {
        LOG_ERROR("Animation not found: {}", animation);
        throw std::runtime_error("Entity::attack: Animation not found: " + animation);
    }
}

bool Entity::update(uint64_t deltaTime) {
    auto entity = m_entity.lock();
    if (!entity) {
        logging::error("Entity::update: Entity is not bound");
        throw std::runtime_error("Entity::update: Entity is not bound");
    }
    if (m_damage_anim.isRunning()) {
        m_damage_anim.update(deltaTime);
    }
    m_animations[m_state]->update(deltaTime);
    return false;
}

void Entity::draw(const graphics::Renderer &renderer) const {
    auto entity = m_entity.lock();
    if (!entity) {
        logging::error("Entity::draw: Entity is not bound");
        throw std::runtime_error("Entity::draw: Entity is not bound");
        return;
    }
    if (m_prev_hp != entity->getHealth()) {
        applyDamage(m_prev_hp - entity->getHealth());
        m_prev_hp = entity->getHealth();
    }

    if (m_draw_stats || m_selection == Selection::kSelected && m_state == "combat")
        drawStats(renderer, {0, 0});

    static std::map<Selection, graphics::Color> selection_colors = {
        {Selection::kNone, 0xffffff},
        {Selection::kSelected, 0xffffff},
        {Selection::kSelectable, 0x666666},
        {Selection::kNotSelectable, 0x222222},
    };

    static std::map<Selection, graphics::Color> battle_selection_colors = {
        {Selection::kNone, 0xffffff},
        {Selection::kSelected, 0xffffff},
        {Selection::kSelectable, 0xaaaaaa},
        {Selection::kNotSelectable, 0x222222},
    };

    graphics::Color selection_color = 0xffffff;
    if (m_state == "idle" || m_state == "walk")
        selection_color = selection_colors.find(m_selection)->second;
    else if (m_state == "combat") {
        selection_color = battle_selection_colors.find(m_selection)->second;
    }

    if (!entity->isAlive()) {
        m_grave->setOrigin(graphics::Sprite::Origin::BOTTOM_CENTER).setColor(selection_color);
        if (m_direction == Direction::kLeft)
            m_grave->setFlip(true, false);
        else if (m_direction == Direction::kRight)
            m_grave->setFlip(false, false);
        renderer.draw(m_grave->setPosition(m_position));
    } else {
        uint64_t current_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        uint64_t delta_time = current_time - m_last_update;
        m_last_update = current_time;
        auto animation = m_animations.find(m_state);
        if (animation != m_animations.end()) {
            auto spine = animation->second;
            spine->update(delta_time);
            spine->setColor(selection_color);
            if (m_direction == Direction::kLeft)
                spine->setFlip(true, false);
            else if (m_direction == Direction::kRight)
                spine->setFlip(false, false);
            spine->setPosition(m_position);
            renderer.draw(spine);
        } else {
            LOG_ERROR("[{}] Animation not found: {}", entity->getId(), m_state);
            throw std::runtime_error("Entity::draw: Animation not found: " + m_state);
        }
    }

    if (m_damage_anim.isRunning()) {
        renderer.draw(m_damage_anim.setPosition(m_position));
    }
    if (m_draw_chance && m_state == "combat" && m_selection == Selection::kSelected) {
        graphics::Text text(std::to_string(static_cast<int>(m_chance)) + "%");
        graphics::Color color =
            graphics::Color::fromHSV(m_chance * 120 / 100, 1, 1);   // 0-120 green, 120-240 yellow, 240-360 red
        text.setFillColor(color);
        text.setOrigin(graphics::Text::Origin::BOTTOM_CENTER).setPosition(m_position + Vector2d(0, -300));
        renderer.draw(text);
    }
}

void Entity::drawStats(const graphics::Renderer &renderer, const Vector2d &position) const {
    auto entity = m_entity.lock();
    if (!entity) {
        logging::error("Entity::drawStats: Entity is not bound");
        throw std::runtime_error("Entity::drawStats: Entity is not bound");
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

Entity &Entity::setState(const std::string &state) {
    m_animations[m_state]->stop();
    m_state = state;
    m_animations[state]->reset();
    if (m_animations[state]->hasAnimation(state))   // [state] == animation name
        m_animations[state]->setAnimation(state);   // set animation [state]
    m_animations[state]->start();
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

Entity &Entity::setDirection(const Direction &direction) {
    m_direction = direction;
    return *this;
}

Entity &Entity::setChance(const float &chance) {
    m_chance = chance;
    return *this;
}

Entity &Entity::setDrawChance(const bool &draw_chance) {
    m_draw_chance = draw_chance;
    return *this;
}

const graphics::SpritePtr &Entity::getPortrait() const {
    return m_portrait;
}

void Entity::reset() {
    m_state = "idle";
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

Entity::Direction Entity::getDirection() const {
    return m_direction;
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

DamageAnimation &DamageAnimation::update(uint64_t deltaTime) {
    m_timer.update(deltaTime);
    return *this;
}

Vector2d Entity::getPosition() const {
    return m_position;
}

}   // namespace views
}   // namespace gui

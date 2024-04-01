#include "animation.h"
#include "logging/logger.h"
#include "sprite.h"
#include "static_data/game_config.h"
#include <fstream>
#include <iostream>
#include <memory>

namespace graphics {

namespace {
std::map<std::string, std::shared_ptr<sf::Texture>> textures;

const std::shared_ptr<sf::Texture> &getTexture(const std::string &path) {
    if (textures.find(path) == textures.end()) {
        textures[path] = std::make_shared<sf::Texture>();
        if (!textures[path]->loadFromFile(cfg::SPRITES_PATH + path)) {
            logging::error("Error loading texture from file: " + cfg::SPRITES_PATH + path);
            throw std::runtime_error("Error loading texture from file: " + cfg::SPRITES_PATH + path);
        }
        logging::debug("Loaded texture " + cfg::SPRITES_PATH + path);
        textures[path]->setSmooth(true);   // enable smooth texture
    }
    return textures[path];
}
}   // namespace

Animation::Animation(const std::string &path) : Sprite(path.substr(0, path.size() - 4) + "/frame-0.png") {
    m_path = path;
    // remove .gif
    m_path = m_path.substr(0, m_path.size() - 4);
    logging::debug("Loading animation " + m_path);
    while (true) {
        std::string frame_path = cfg::SPRITES_PATH + m_path + "/frame-" + std::to_string(m_frames_count) + ".png";
        if (std::ifstream(frame_path).good()) {
            m_frames_count++;
        } else {
            break;
        }
    }
    if (m_frames_count == 0) {
        logging::error("No frames found in animation " + m_path);
        throw std::runtime_error("No frames found in animation " + m_path);
    }
    logging::debug("Loaded " + std::to_string(m_frames_count) + " frames for animation " + m_path);
    for (uint32_t i = 0; i < m_frames_count; i++) {
        std::string frame_path = m_path + "/frame-" + std::to_string(i) + ".png";
        m_textures.push_back(getTexture(frame_path));
    }
    logging::debug("Loaded animation " + m_path);
}

const uint32_t Animation::getFramesCount() const {
    return m_frames_count;
}

std::shared_ptr<sf::Texture> Animation::getFrame() const {
    uint64_t time =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    uint32_t frame = (time / cfg::ANIMATION_FRAME_DURATION) % m_frames_count;
    if (frame < m_frames_count) {
        return m_textures[frame];
    } else {
        std::cerr << "Frame " << frame << " not found in animation " << m_path << std::endl;
        return nullptr;
    }
}

void Animation::draw(const Renderer &renderer) const {
    m_sprite.setTexture(*getFrame());
    Sprite::draw(renderer);
}

}   // namespace graphics

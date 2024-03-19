#include "animation.h"
#include "sprite.h"
#include "static_data/game_config.h"
#include <fstream>
#include <iostream>
#include <memory>

namespace graphics {

Animation::Animation(const std::string &path) {
    m_path = path;
    // remove .gif
    m_path = m_path.substr(0, m_path.size() - 4);
    while (true) {
        std::string frame_path = cfg::SPRITES_PATH + m_path + "/frame-" + std::to_string(m_frames_count) + ".png";
        if (std::ifstream(frame_path).good()) {
            m_frames_count++;
        } else {
            break;
        }
    }
    printf("Loaded animation %s with %d frames\n", m_path.c_str(), m_frames_count);
    for (uint32_t i = 0; i < m_frames_count; i++) {
        std::string frame_path = m_path + "/frame-" + std::to_string(i) + ".png";
        m_frames.push_back(Sprite::load(frame_path));
    }
}

const uint32_t Animation::getFramesCount() const {
    return m_frames_count;
}

SpritePtr Animation::getFrame() const {
    uint64_t time =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    uint32_t frame = (time / cfg::ANIMATION_FRAME_DURATION) % m_frames_count;
    if (frame < m_frames_count) {
        return m_frames[frame];
    } else {
        std::cerr << "Frame " << frame << " not found in animation " << m_path << std::endl;
        return nullptr;
    }
}

}   // namespace graphics

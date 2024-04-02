#pragma once
#include <SFML/Audio.hpp>
#include <string>

namespace sound {
class SoundManager {
public:
    static SoundManager &getInstance() {
        static SoundManager instance;
        return instance;
    }

    void playSound(const std::string &sound_name, const float volume) {
        sound_buffer = std::make_unique<sf::SoundBuffer>();
        sound = std::make_unique<sf::Sound>();
        if (sound_buffer->loadFromFile(cfg::SOUNDS_PATH + sound_name + ".wav")) {
            sound->setBuffer(*sound_buffer);
            sound->setVolume(volume);
            sound->play();
        }
    }

private:
    SoundManager() = default;

    std::unique_ptr<sf::SoundBuffer> sound_buffer;
    std::unique_ptr<sf::Sound> sound;
};

static void playSound(const std::string &sound_name, float volume = 100.f) {
    sound::SoundManager::getInstance().playSound(sound_name, volume);
}
}   // namespace sound

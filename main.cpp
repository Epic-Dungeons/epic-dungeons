#include "dungeon/makers/dungeon-maker.h"
#include "graphics/graphics.h"
#include "gui_controller/controller.h"
#include "keyboard/keyboard.h"
#include "logging/logger.h"
#include "static_data/game_config.h"
#include <SFML/Graphics.hpp>
#include <chrono>

// get -v from the command line
int main(int argc, char *argv[]) {
#ifdef DEBUG
    for (int i = 0; i < argc; i++) {
        if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") {
            logging::setLevel(spdlog::level::debug);
            logging::debug("Verbose mode enabled");
        }
        if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
            std::cout << "Usage: " << argv[0] << " [-v|--verbose] [-h|--help]" << std::endl;
            return 0;
        }
    }
#endif

    auto window = sf::RenderWindow {{cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT},
                                    cfg::WINDOW_NAME,
                                    sf::Style::Titlebar | sf::Style::Close};
    window.setFramerateLimit(cfg::FRAMERATE);
    sf::Clock clock;
    // window.setVerticalSyncEnabled(true);

    auto renderer = std::make_shared<graphics::Renderer>(window);
    auto engine = std::make_shared<engine::Engine>();

    dungeon::DungeonMaker dungeon_maker;
    dungeon_maker.setRandomSeed();
    dungeon_maker.build();
    engine->bindDungeon(dungeon_maker.getDungeon());

    gui::Controller controller;
    controller.bindEngine(engine);
    controller.bindRenderer(renderer);
    uint64_t last_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    int rounded_fps = 0;
    const int frames_to_average = 5;
    uint64_t accumulated_time = 0;
    int frames_counter = 0;
    double alpha = 0.1;
    double smoothed_fps = 0.0;

    clock.restart();
    while (window.isOpen()) {
        uint64_t current_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        uint64_t delta_time = current_time - last_time;

        if (delta_time < 1000.0f / cfg::FRAMERATE) {
            continue;
        }
        frames_counter++;
        accumulated_time += delta_time;

        if (frames_counter == frames_to_average) {
            uint64_t average_time = accumulated_time / frames_to_average;

            if (average_time != 0) {
                if (smoothed_fps == 0) {
                    smoothed_fps = 1000 / average_time;
                } else {
                    smoothed_fps = alpha * (1000 / average_time) + (1 - alpha) * smoothed_fps;
                }
            } else {
                smoothed_fps = 0;
            }
            rounded_fps = std::floor(smoothed_fps + 0.5);

            frames_counter = 0;
            accumulated_time = 0;
        }

        last_time = current_time;

        controller.setDeltaTime(delta_time);

        for (auto event = sf::Event {}; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                keyboard::setKeyState(event.key.code, true);
            }
            if (event.type == sf::Event::KeyReleased) {
                keyboard::setKeyState(event.key.code, false);
            }
        }
        controller.update();
        if (cfg::FPS_COUNTER) {
            window.setTitle(cfg::WINDOW_NAME + " | " + std::to_string(rounded_fps) + " FPS");
        } else {
            window.setTitle(cfg::WINDOW_NAME);
        }
    }
    return 0;
}

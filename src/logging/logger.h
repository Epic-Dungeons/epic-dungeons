// logger.h
#pragma once
#include "static_data/game_config.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#define LOG_DEBUG(...) logging::debug(__VA_ARGS__)
#define LOG_INFO(...) logging::info(__VA_ARGS__)
#define LOG_WARNING(...) logging::warn(__VA_ARGS__)
#define LOG_ERROR(...) logging::error(__VA_ARGS__)

namespace logging {

class Logger {
public:
    Logger() {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true));
        m_logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
        spdlog::register_logger(m_logger);
        m_logger->set_level(cfg::LOG_LEVEL);
        // setup format
        spdlog::set_pattern("[%H:%M:%S %z] [%^%l%$] %v");
    }

    void debug(const std::string &msg) {
        m_logger->debug(msg);
    }

    template<typename... Args>
    void debug(spdlog::format_string_t<Args...> msg, Args &&...args) {
        m_logger->debug(msg, std::forward<Args>(args)...);
    }

    void info(const std::string &msg) {
        m_logger->info(msg);
    }

    template<typename... Args>
    void info(spdlog::format_string_t<Args...> msg, Args &&...args) {
        m_logger->info(msg, std::forward<Args>(args)...);
    }

    void warn(const std::string &msg) {
        m_logger->warn(msg);
    }

    template<typename... Args>
    void warn(spdlog::format_string_t<Args...> msg, Args &&...args) {
        m_logger->warn(msg, std::forward<Args>(args)...);
    }

    void error(const std::string &msg) {
        m_logger->error(msg);
    }

    template<typename... Args>
    void error(spdlog::format_string_t<Args...> msg, Args &&...args) {
        m_logger->error(msg, std::forward<Args>(args)...);
    }

    void setLevel(const spdlog::level::level_enum &level) {
        m_logger->set_level(level);
    }

    static Logger &getLogger() {
        static Logger logger;
        return logger;
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
};

static void debug(const std::string &msg) {
    Logger::getLogger().debug(msg);
}

template<typename... Args>
static void debug(spdlog::format_string_t<Args...> msg, Args &&...args) {
    Logger::getLogger().debug(msg, std::forward<Args>(args)...);
}

static void info(const std::string &msg) {
    Logger::getLogger().info(msg);
}

template<typename... Args>
static void info(spdlog::format_string_t<Args...> msg, Args &&...args) {
    Logger::getLogger().info(msg, std::forward<Args>(args)...);
}

static void warn(const std::string &msg) {
    Logger::getLogger().warn(msg);
}

template<typename... Args>
static void warn(spdlog::format_string_t<Args...> msg, Args &&...args) {
    Logger::getLogger().warn(msg, std::forward<Args>(args)...);
}

static void error(const std::string &msg) {
    Logger::getLogger().error(msg);
}

template<typename... Args>
static void error(spdlog::format_string_t<Args...> msg, Args &&...args) {
    Logger::getLogger().error(msg, std::forward<Args>(args)...);
}

static void setLevel(const spdlog::level::level_enum &level) {
    Logger::getLogger().setLevel(level);
}

}   // namespace logging

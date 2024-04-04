#include "timed_count.h"
#include <chrono>
#include <cstdint>
#include <utility>

namespace gui {
void TimedCount::init(float from, float to, uint64_t time) {
    m_from = from;
    m_to = to;
    m_time = time;
    m_cur_time = m_time;   // set to end
    m_f = [](float x) {
        return x;
    };
}

void TimedCount::init(float from, float to, uint64_t time, std::function<float(float)> f) {
    m_from = from;
    m_to = to;
    m_time = time;
    m_cur_time = m_time;   // set to end
    m_f = std::move(f);
    is_started = false;
}

void TimedCount::setFunction(std::function<float(float)> f) {
    m_f = std::move(f);
}

void TimedCount::start() {
    m_cur_time = 0;
    m_last_time = getCurTime();
    is_started = true;
}

const uint64_t TimedCount::getCurTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
}

void TimedCount::update() {
    uint64_t delta_time = getCurTime() - m_last_time;
    m_last_time = getCurTime();
    update(delta_time);
};

void TimedCount::update(uint64_t delta_time) {
    m_cur_time += delta_time;
    if (m_cur_time > m_time) {
        m_cur_time = m_time;
    }
}

void TimedCount::reset() {
    m_cur_time = m_time;
    is_started = false;
}

float TimedCount::get() const {
    return m_from + m_f(static_cast<float>(m_cur_time) / static_cast<float>(m_time)) * (m_to - m_from);
}

bool TimedCount::isEnded() const {
    return m_cur_time == m_time;
}

bool TimedCount::isStarted() const {
    return is_started;
}

}   // namespace gui

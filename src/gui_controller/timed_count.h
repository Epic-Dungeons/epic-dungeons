#pragma once
#include <chrono>
#include <cstdint>
#include <functional>

namespace gui {

class TimedCount {
public:
    void init(float from, float to, uint64_t time);

    void init(float from, float to, uint64_t time, std::function<float(float)> f);

    void start();
    void update();
    void update(uint64_t delta_time);
    void setFunction(std::function<float(float)> f);

    const uint64_t getCurTime();

    float get() const;

    bool isEnded() const;

private:
    float m_from, m_to;
    uint64_t m_time;
    uint64_t m_cur_time;
    uint64_t m_last_time;
    std::function<float(float)> m_f;
};
}   // namespace gui

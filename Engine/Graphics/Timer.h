#pragma once

#include <chrono>

class Timer {
public:
    void Initialize();
    void KeepFrameRate(uint32_t fps);

private:
    std::chrono::steady_clock::time_point reference_;
};
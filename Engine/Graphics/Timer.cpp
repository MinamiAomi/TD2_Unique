#include "Timer.h"

using namespace std::chrono;

void Timer::Initialize() {
    reference_ = steady_clock::now();
}

void Timer::KeepFrameRate(uint32_t fps) {
    steady_clock::time_point now = steady_clock::now();
    microseconds elapsed = duration_cast<microseconds>(now - reference_);
    const microseconds minCheckTime(uint64_t(1000000.0f / (float(fps) + 2.0f)));
    const microseconds minTime(uint64_t(1000000.0f / float(fps)));
    microseconds check = minCheckTime - elapsed;
    if (check > microseconds::zero()) {
        microseconds waitTime = minTime - elapsed;

        steady_clock::time_point waitStart = steady_clock::now();
        do {
            std::this_thread::sleep_for(microseconds(1));
        } while (steady_clock::now() - waitStart < waitTime);
    }
    reference_ = steady_clock::now();
}

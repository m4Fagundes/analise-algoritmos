// src/core/Timer.h

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    double elapsed_milliseconds() {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end_time - start_time_;
        return elapsed.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
};

#endif // TIMER_H
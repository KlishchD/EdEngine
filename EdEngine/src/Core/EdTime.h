#pragma once

#include <chrono>

using Clock = std::chrono::system_clock;
using TimeDuration = std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period>;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
using DurationType = float;

inline DurationType DurationAsSeconds(TimeDuration duration)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0f;
}
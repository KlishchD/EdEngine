#pragma once

using Clock = std::chrono::system_clock;
using TimeDuration = std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period>;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
using DurationType = f32;

DurationType DurationAsSeconds(TimeDuration duration);
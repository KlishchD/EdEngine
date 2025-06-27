#include "EdCore.h"
#include "EdTime.h"

DurationType DurationAsSeconds(TimeDuration duration)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0f;
}

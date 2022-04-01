#include "SimulationFrameTime.h"

#include <cmath>

float SimulationFrameTime::indexToTime(const uint32_t frame, const float start, const float end, const float dt)
{
    const auto upRoundedDt = std::nextafter(dt, std::numeric_limits<float>::infinity());
    auto timeStamp = start + frame * upRoundedDt;

    timeStamp = std::min(std::max(timeStamp, start), end);

    return timeStamp;
}

#pragma once

#include <cstdint>

struct SimulationFrameTime
{
    static float indexToTime(const uint32_t frame, const float start, const float end, const float dt);
};

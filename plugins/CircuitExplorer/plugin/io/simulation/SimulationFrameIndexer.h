#pragma once

#include <cstdint>
#include <vector>

struct SimulationFrameIndexer
{
    static void updateIndexing(const std::vector<float> &frame,
                               const std::vector<uint64_t> &offsets,
                               const float rangeStart,
                               const float rangeEnd,
                               std::vector<uint8_t> &indices);
};

#pragma once

#include <brayns/common/MathTypes.h>

#include <cstdint>
#include <vector>

struct SimulationFrameIndexer
{
    static std::vector<uint8_t> computeIndices(const std::vector<float> &frame,
                                               const std::vector<uint64_t> &offsets,
                                               const brayns::Vector2f &range) noexcept;

    static std::vector<uint8_t> computeIndices(const std::vector<float> &frame,
                                               const brayns::Vector2f &range) noexcept;
};

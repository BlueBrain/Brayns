#include "SimulationFrameIndexer.h"

#include <cmath>

std::vector<uint8_t> SimulationFrameIndexer::computeIndices(const std::vector<float> &frame,
                                                            const std::vector<uint64_t> &offsets,
                                                            const brayns::Vector2f &range) noexcept
{
    const auto rangeStart = range.x;
    const auto rangeEnd = range.y;
    const auto invFactor = 1.f / std::fabs(rangeEnd - rangeStart);

    std::vector<uint8_t> indices (offsets.size());

#pragma omp parallel for
    for(size_t i = 0; i < offsets.size(); ++i)
    {
        const auto offset = offsets[i];
        auto value = frame[offset];
        value = value > rangeEnd? rangeEnd : (value < rangeStart? rangeStart : value);

        const auto normIndex = (value - rangeStart) * invFactor;
        indices[i] = static_cast<uint8_t>(normIndex * 256);
    }

    return indices;
}

std::vector<uint8_t> SimulationFrameIndexer::computeIndices(const std::vector<float> &frame,
                                                            const brayns::Vector2f &range) noexcept
{
    const auto rangeStart = range.x;
    const auto rangeEnd = range.y;
    const auto invFactor = 1.f / std::fabs(rangeEnd - rangeStart);

    std::vector<uint8_t> indices (frame.size());

#pragma omp parallel for
    for(size_t i = 0; i < frame.size(); ++i)
    {
        auto value = frame[i];
        value = value > rangeEnd? rangeEnd : (value < rangeStart? rangeStart : value);

        const auto normIndex = (value - rangeStart) * invFactor;
        indices[i] = static_cast<uint8_t>(normIndex * 256);
    }

    return indices;
}


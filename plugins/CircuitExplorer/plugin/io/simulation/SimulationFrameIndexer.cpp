#include "SimulationFrameIndexer.h"

#include <cmath>

void SimulationFrameIndexer::updateIndexing(const std::vector<float> &frame,
                                            const std::vector<uint64_t> &offsets,
                                            const float rangeStart,
                                            const float rangeEnd,
                                            std::vector<uint8_t> &indices)
{
    const auto invFactor = 1.f / std::fabs(rangeEnd - rangeStart);

#pragma omp parallel for
    for(size_t i = 0; i < offsets.size(); ++i)
    {
        const auto offset = offsets[i];
        auto value = frame[offset];
        value = value > rangeEnd? rangeEnd : (value < rangeStart? rangeStart : value);

        const auto normIndex = (value - rangeStart) * invFactor;
        indices[i] = static_cast<uint8_t>(normIndex * 256);
    }
}

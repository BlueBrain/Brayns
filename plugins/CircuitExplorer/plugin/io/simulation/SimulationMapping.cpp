#include "SimulationMapping.h"

#include <cmath>

std::vector<uint64_t> SimulationMappingGenerator::generate(const std::vector<CompartmentStructure> &structure,
                                                           const std::vector<SimulationMapping> &mapping)
{
    std::vector<uint64_t> result;
    size_t resultSize = 0;
    for(const auto &compartment : structure)
    {
        resultSize += compartment.numItems;
    }
    result.reserve(resultSize);

    for(size_t i = 0; i < structure.size(); ++i)
    {
        const auto size = structure[i].numItems;
        const auto &comparments = structure[i].sectionSegments;

        const auto &map = mapping[i];
        const auto offset = map.globalOffset;
        const auto &localOffsets = map.offsets;
        const auto &compartments = map.compartments;

        std::vector<uint64_t> localResult (size, offset);

        for (const auto& [sectionId, segments] : comparments)
        {
            // No section level information (soma report, spike simulation, etc.) or dealing with soma
            if (sectionId <= -1 || localOffsets.empty() || static_cast<size_t>(sectionId) > localOffsets.size() - 1)
            {
                continue;
            }
            else
            {
                const auto numSegments = segments.size();
                const auto numCompartments = compartments[sectionId];
                const auto step = static_cast<double>(numCompartments) / static_cast<double>(numSegments);
                const size_t sectionOffset = localOffsets[sectionId];
                for (size_t i = 0; i < segments.size(); ++i)
                {
                    const auto compartment = static_cast<size_t>(std::floor(step * i));
                    const auto finalOffset = offset + (sectionOffset + compartment);
                    const auto segmentIndex = segments[i];
                    localResult[segmentIndex] = finalOffset;
                }
            }
        }

        result.insert(result.end(), localResult.begin(), localResult.end());
    }

    return result;
}

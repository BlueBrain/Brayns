#include "ReportMapping.h"

std::vector<size_t> CompartmentMappingGenerator::generate(
    const std::vector<CellCompartments> &structure,
    const std::vector<CellReportMapping> &mapping)
{
    std::vector<uint64_t> result;
    size_t resultSize = 0;
    for (const auto &compartment : structure)
    {
        resultSize += compartment.numItems;
    }
    result.reserve(resultSize);

    for (size_t comp = 0; comp < structure.size(); ++comp)
    {
        const auto &cellStructure = structure[comp];
        const auto size = cellStructure.numItems;
        const auto &comparments = cellStructure.sectionSegments;

        const auto &map = mapping[comp];
        const auto offset = map.globalOffset;
        const auto &localOffsets = map.offsets;
        const auto &compartments = map.compartments;

        std::vector<uint64_t> localResult(size, offset);

        for (const auto &[sectionId, segments] : comparments)
        {
            // No section level information (soma report, spike simulation, etc.) or dealing with soma
            if (sectionId < 0 || localOffsets.empty() || static_cast<size_t>(sectionId) >= localOffsets.size())
            {
                continue;
            }

            const auto numSegments = segments.size();
            const auto numCompartments = compartments[sectionId];
            const auto step = float(numCompartments) / float(numSegments);
            const size_t sectionOffset = localOffsets[sectionId];
            for (size_t i = 0; i < segments.size(); ++i)
            {
                const auto compartment = static_cast<size_t>(step * i);
                const auto finalOffset = offset + sectionOffset + compartment;
                const auto segmentIndex = segments[i];
                localResult[segmentIndex] = finalOffset;
            }
        }

        result.insert(result.end(), localResult.begin(), localResult.end());
    }

    return result;
}

std::vector<size_t> ElementMappingGenerator::generate(
    const std::vector<uint64_t> &elementIds,
    const std::unordered_map<uint64_t, size_t> &mapping)
{
    std::vector<size_t> result(elementIds.size());

    for (size_t i = 0; i < elementIds.size(); ++i)
    {
        const auto elementId = elementIds[i];

        auto it = mapping.find(elementId);
        if (it == mapping.end())
        {
            throw std::runtime_error("No report mapping information for element " + std::to_string(elementId));
        }

        const auto &entry = *it;
        const auto offset = entry.second;
        result[i] = offset;
    }

    return result;
}

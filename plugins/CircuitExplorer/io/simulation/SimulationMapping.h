#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

/**
 * @brief The CompartmentStructure struct represents the internal structure of an element onto which simulation
 * will be mapped
 */
struct CompartmentStructure
{
    size_t numItems {};
    std::unordered_map<int32_t, std::vector<size_t>> sectionSegments;
};

struct SimulationMapping
{
    // Global offset within the while simulation frame
    size_t globalOffset;
    // For each section (0...N), relative offset from globalOffset
    std::vector<uint16_t> offsets;
    // For each section (0...N), number of compartments
    std::vector<uint16_t> compartments;
};

struct SimulationMappingGenerator
{
    static std::vector<uint64_t> generate(const std::vector<CompartmentStructure> &structure,
                                          const std::vector<SimulationMapping> &mapping);
};

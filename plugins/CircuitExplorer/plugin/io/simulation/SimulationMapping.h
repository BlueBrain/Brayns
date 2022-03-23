#pragma once

#include <cstdint>
#include <vector>

struct SimulationMapping
{
    // Global offset within the while simulation frame
    size_t globalOffset;
    // For each section (0...N), relative offset from globalOffset
    std::vector<uint16_t> offsets;
    // For each section (0...N), number of compartments
    std::vector<uint16_t> compartments;
};

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

/**
 * @brief The CellCompartments represents the internal structure of an element onto which simulation will be mapped
 */
struct CellCompartments
{
    size_t numItems{};
    std::unordered_map<int32_t, std::vector<size_t>> sectionSegments;
};

/**
 * @brief The CellReportMapping holds the report compartment mapping of a cell
 *
 */
struct CellReportMapping
{
    // Global offset within the while simulation frame
    size_t globalOffset;
    // For each section (0...N), relative offset from globalOffset
    std::vector<uint16_t> offsets;
    // For each section (0...N), number of compartments
    std::vector<uint16_t> compartments;
};

/**
 * @brief Report mapping generation for compartmented elements
 */
struct CompartmentMappingGenerator
{
    /**
     * @brief Generates the array of offsets into the report frame for a set of compartmented elements
     *
     * @param structure
     * @param mapping
     * @return std::vector<size_t>
     */
    static std::vector<size_t> generate(
        const std::vector<CellCompartments> &structure,
        const std::vector<CellReportMapping> &mapping);
};

/**
 * @brief Report mapping generation for opaque individual elements
 */
struct ElementMappingGenerator
{
    /**
     * @brief Generates the array of offsets into the report frame for a set of opaque elements
     *
     * @param elementIds
     * @param mapping
     * @return std::vector<size_t>
     */
    static std::vector<size_t> generate(
        const std::vector<uint64_t> &elementIds,
        const std::unordered_map<uint64_t, size_t> &mapping);
};

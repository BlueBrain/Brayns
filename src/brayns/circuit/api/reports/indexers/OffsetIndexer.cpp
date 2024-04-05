/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "OffsetIndexer.h"

namespace
{
class OffsetGenerator
{
public:
    static auto generate(const std::vector<CellCompartments> &structure, const std::vector<CellReportMapping> &mapping)
    {
        std::vector<uint64_t> offsets;
        auto totalComparments = _computeTotalComparmentsCount(structure);
        offsets.reserve(totalComparments);

        for (size_t comp = 0; comp < structure.size(); ++comp)
        {
            const auto &cellStructure = structure[comp];
            const auto &cellReportMapping = mapping[comp];

            auto cellOffsets = _computeCellCompartmentOffsets(cellStructure, cellReportMapping);

            offsets.insert(offsets.end(), cellOffsets.begin(), cellOffsets.end());
        }

        return offsets;
    }

private:
    static size_t _computeTotalComparmentsCount(const std::vector<CellCompartments> &structure)
    {
        size_t resultSize = 0;
        for (const auto &compartment : structure)
        {
            resultSize += compartment.numItems;
        }
        return resultSize;
    }

    static std::vector<uint64_t> _computeCellCompartmentOffsets(
        const CellCompartments &cellStructure,
        const CellReportMapping &cellReportMapping)
    {
        auto size = cellStructure.numItems;
        auto &sectionsSegments = cellStructure.sectionSegments;

        auto offset = cellReportMapping.globalOffset;
        auto &localOffsets = cellReportMapping.offsets;
        auto &compartments = cellReportMapping.compartments;

        std::vector<uint64_t> localResult(size, offset);

        for (auto &section : sectionsSegments)
        {
            // No section level information (soma report, spike simulation, etc.) or dealing with soma
            if (section.sectionId < 0 || localOffsets.empty()
                || static_cast<size_t>(section.sectionId) >= localOffsets.size())
            {
                continue;
            }

            auto numCompartments = compartments[section.sectionId];
            if (numCompartments == 0)
            {
                continue;
            }

            auto numSegments = section.end - section.begin;
            auto step = static_cast<float>(numCompartments) / static_cast<float>(numSegments);
            auto sectionOffset = localOffsets[section.sectionId];

            for (size_t i = 0; i < numSegments; ++i)
            {
                auto index = static_cast<float>(i);
                auto compartment = static_cast<size_t>(step * index);
                auto finalOffset = offset + sectionOffset + compartment;
                localResult[section.begin + i] = finalOffset;
            }
        }

        return localResult;
    }
};
}

OffsetIndexer::OffsetIndexer(std::vector<size_t> offsets):
    _offsets(std::move(offsets))
{
}

OffsetIndexer::OffsetIndexer(
    const std::vector<CellCompartments> &structure,
    const std::vector<CellReportMapping> &mapping):
    _offsets(OffsetGenerator::generate(structure, mapping))
{
}

std::vector<uint8_t> OffsetIndexer::generate(const std::vector<float> &data, const brayns::Vector2f &range) noexcept
{
    std::vector<uint8_t> indices(_offsets.size());

    auto rangeStart = range.x;
    auto rangeEnd = range.y;
    auto invFactor = 1.f / std::fabs(rangeEnd - rangeStart);

#pragma omp parallel for
    for (size_t i = 0; i < _offsets.size(); ++i)
    {
        auto offset = _offsets[i];
        auto value = data[offset];
        value = value > rangeEnd ? rangeEnd : (value < rangeStart ? rangeStart : value);

        auto normIndex = (value - rangeStart) * invFactor;
        indices[i] = static_cast<uint8_t>(normIndex * 255.f);
    }

    return indices;
}

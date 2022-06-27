/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
        const auto size = cellStructure.numItems;
        const auto &compartmentMap = cellStructure.sectionSegments;

        const auto offset = cellReportMapping.globalOffset;
        const auto &localOffsets = cellReportMapping.offsets;
        const auto &compartments = cellReportMapping.compartments;

        std::vector<uint64_t> localResult(size, offset);

        for (const auto &[sectionId, segments] : compartmentMap)
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

        return localResult;
    }
};
}

OffsetIndexer::OffsetIndexer(std::vector<size_t> offsets)
    : _offsets(std::move(offsets))
{
}

OffsetIndexer::OffsetIndexer(
    const std::vector<CellCompartments> &structure,
    const std::vector<CellReportMapping> &mapping)
    : _offsets(OffsetGenerator::generate(structure, mapping))
{
}

std::vector<uint8_t> OffsetIndexer::generate(const std::vector<float> &data, const brayns::Vector2f &range) noexcept
{
    std::vector<uint8_t> indices(_offsets.size());

    const auto rangeStart = range.x;
    const auto rangeEnd = range.y;
    const auto invFactor = 1.f / std::fabs(rangeEnd - rangeStart);

    for (size_t i = 0; i < _offsets.size(); ++i)
    {
        const auto offset = _offsets[i];
        auto value = data[offset];
        value = value > rangeEnd ? rangeEnd : (value < rangeStart ? rangeStart : value);

        const auto normIndex = (value - rangeStart) * invFactor;
        indices[i] = static_cast<uint8_t>(normIndex * 255.f);
    }

    return indices;
}

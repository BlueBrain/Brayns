/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "NodeCompartmentLoader.h"

#include <plugin/io/sonataloader/data/SonataSimulationMapping.h>

namespace sonataloader
{
std::vector<NodeReportMapping> NodeCompartmentLoader::loadMapping(
    const std::string& reportPath, const std::string& population,
    const bbp::sonata::Selection& s) const
{
    const auto rawMapping =
        SonataSimulationMapping::getCompartmentMapping(reportPath, population,
                                                       s.flatten());
    // Compact mapping
    std::map<uint64_t, std::vector<uint16_t>> sortedCompartmentsSize;
    uint32_t lastSection = std::numeric_limits<uint32_t>::max();
    uint64_t lastNode = std::numeric_limits<uint64_t>::max();
    for (const auto& key : rawMapping)
    {
        auto& cm = sortedCompartmentsSize[key.first];
        if (lastSection != key.second || lastNode != key.first)
        {
            lastNode = key.first;
            lastSection = key.second;
            cm.push_back(0u);
        }
        cm[key.second]++;
    }

    // Returns a node id sorted list of compartment mappings
    std::vector<NodeReportMapping> mapping(sortedCompartmentsSize.size());
    // Transform into brayns mapping
    auto it = sortedCompartmentsSize.begin();
    size_t index = 0;
    size_t prevOffset = 0;
    for (; it != sortedCompartmentsSize.end(); ++it)
    {
        auto& cellMapping = mapping[index];
        cellMapping.globalOffset = prevOffset;
        cellMapping.compartments.resize(it->second.size());
        cellMapping.offsets.resize(it->second.size());

        uint16_t localOffset = 0;
        for (size_t i = 0; i < it->second.size(); ++i)
        {
            const auto sectionCompartments = it->second[i];
            cellMapping.offsets[i] = localOffset;
            cellMapping.compartments[i] = sectionCompartments;
            localOffset += sectionCompartments;
            prevOffset += sectionCompartments;
        }

        ++index;
    }

    return mapping;
}
} // namespace sonataloader

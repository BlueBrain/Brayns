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

#include "EdgeCompartmentLoader.h"

#include <plugin/io/sonataloader/data/SonataSimulationMapping.h>

#include <bbp/sonata/report_reader.h>

namespace sonataloader
{
std::vector<EdgeReportMapping> EdgeCompartmentLoader::loadMapping(
    const std::string& reportPath, const std::string& population,
    const bbp::sonata::Selection& s) const
{
    const auto nodeIds = s.flatten();
    const auto rawMapping =
        SonataSimulationMapping::getCompartmentMapping(reportPath, population,
                                                       nodeIds);
    // Pre-fill the map so all node Ids will have their mapping,
    // even if these nodes are not reported on the simulation
    std::map<uint64_t, EdgeReportMapping> sortedCompartmentsSize;
    for (const auto nodeId : nodeIds)
        sortedCompartmentsSize[nodeId] = {};

    // Gather mapping
    uint64_t offset = 0;
    for (const auto& key : rawMapping)
    {
        auto& nodeMapping = sortedCompartmentsSize[key.first];
        nodeMapping.offsets[key.second] = offset++;
    }

    // Flatten
    std::vector<EdgeReportMapping> mapping(sortedCompartmentsSize.size());
    auto it = sortedCompartmentsSize.begin();
    for (size_t index = 0; it != sortedCompartmentsSize.end(); ++it, ++index)
        mapping[index] = std::move(it->second);

    return mapping;
}
} // namespace sonataloader

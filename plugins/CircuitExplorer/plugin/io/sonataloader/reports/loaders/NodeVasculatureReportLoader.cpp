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

#include "NodeVasculatureReportLoader.h"

#include <plugin/io/sonataloader/data/SonataSimulationMapping.h>

namespace sonataloader
{
std::vector<NodeReportMapping> NodeVasculatureReportLoader::loadMapping(
    const std::string& reportPath, const std::string& population,
    const bbp::sonata::Selection& s) const
{
    const auto rawMapping =
        SonataSimulationMapping::getCompartmentMapping(reportPath, population,
                                                       s.flatten());

    // Sorted vasculature mapping indices
    std::map<uint64_t, size_t> sortedCompartmentsSize;
    for (size_t i = 0; i < rawMapping.size(); ++i)
        sortedCompartmentsSize[rawMapping[i].first] = i;

    // Flatten
    std::vector<NodeReportMapping> mapping(sortedCompartmentsSize.size());
    auto it = sortedCompartmentsSize.begin();
    size_t index = 0;
    while (it != sortedCompartmentsSize.end())
    {
        mapping[index].globalOffset = it->second;
        ++it;
        ++index;
    }

    return mapping;
}
} // namespace sonataloader

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

#include "CompartmentSimulation.h"

#include <plugin/io/bbploader/simulation/handlers/CompartmentHandler.h>

namespace bbploader
{
CompartmentSimulation::CompartmentSimulation(const std::string& path,
                                             const brain::GIDSet& inputGids)
    : _path(path)
    , _report(std::make_shared<brion::CompartmentReport>(brion::URI(path),
                                                         brion::MODE_READ,
                                                         inputGids))
{
}

const brain::GIDSet& CompartmentSimulation::getReportGids() const
{
    return _report->getGIDs();
}

std::vector<Simulation::CellMapping> CompartmentSimulation::getMapping(
    const brain::GIDSet& inputGids) const
{
    const auto& ccounts = _report->getCompartmentCounts();
    const auto& offsets = _report->getOffsets();

    std::vector<Simulation::CellMapping> mapping(inputGids.size());

#pragma omp parallel for
    for (size_t i = 0; i < inputGids.size(); ++i)
    {
        const auto& count = ccounts[i];
        const auto& offset = offsets[i];

        mapping[i].globalOffset = offset[0];

        mapping[i].compartments =
            std::vector<uint16_t>(count.begin(), count.end());

        mapping[i].offsets.resize(offset.size());
        for (size_t j = 0; j < offset.size(); ++j)
            mapping[i].offsets[j] = offset[j] - mapping[i].globalOffset;
    }

    return mapping;
}

brayns::AbstractSimulationHandlerPtr CompartmentSimulation::createHandler()
    const
{
    return std::make_shared<CompartmentHandler>(_path, _report);
}
} // namespace bbploader

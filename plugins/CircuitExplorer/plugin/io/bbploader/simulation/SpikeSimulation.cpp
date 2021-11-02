/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "SpikeSimulation.h"

#include <plugin/io/bbploader/simulation/handlers/SpikeHandler.h>

namespace bbploader
{
SpikeSimulation::SpikeSimulation(const std::string& reportPath,
                                 const brain::GIDSet& inputGids, const float tt)
    : _path(reportPath)
    , _transitionTime(tt)
    , _gids(inputGids)
    , _report(std::make_shared<brain::SpikeReportReader>(brion::URI(reportPath),
                                                         inputGids))
{
}

const brain::GIDSet& SpikeSimulation::getReportGids() const
{
    return _gids;
}

std::vector<Simulation::CellMapping> SpikeSimulation::getMapping(
    const brain::GIDSet& inputGids) const
{
    std::vector<Simulation::CellMapping> result(inputGids.size());
    for (size_t i = 0; i < inputGids.size(); ++i)
        result[i].globalOffset = i;
    return result;
}

brayns::AbstractSimulationHandlerPtr SpikeSimulation::createHandler() const
{
    return std::make_shared<SpikeHandler>(_path, _transitionTime, _gids,
                                          _report);
}
} // namespace bbploader

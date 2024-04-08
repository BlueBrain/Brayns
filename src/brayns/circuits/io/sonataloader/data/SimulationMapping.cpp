/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "SimulationMapping.h"

namespace sonataloader
{
std::vector<bbp::sonata::NodeID> SimulationMapping::getCompartmentNodes(
    const std::string &reportPath,
    const std::string &population)
{
    auto report = bbp::sonata::ElementReportReader(reportPath);
    auto &reportPopulation = report.openPopulation(population);
    return reportPopulation.getNodeIds();
}

std::vector<bbp::sonata::CompartmentID> SimulationMapping::getCompartmentMapping(
    const std::string &reportPath,
    const std::string &population,
    const std::vector<bbp::sonata::NodeID> &nodeIds)
{
    auto report = bbp::sonata::ElementReportReader(reportPath);
    auto &reportPopulation = report.openPopulation(population);
    auto selection = bbp::sonata::Selection::fromValues(nodeIds);
    return reportPopulation.getNodeIdElementIdMapping(selection);
}
} // namespace sonataloader

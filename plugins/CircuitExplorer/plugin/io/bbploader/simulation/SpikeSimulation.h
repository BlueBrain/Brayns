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

#pragma once

#include <plugin/io/bbploader/simulation/Simulation.h>

namespace bbploader
{
/**
 * @brief The SpikeSimulation class implements BBP internal format spike reports
 */
class SpikeSimulation : public Simulation
{
public:
    SpikeSimulation(const std::string& reportPath,
                    const brain::GIDSet& inputGids, const float tt);

    const brain::GIDSet& getReportGids() const final;
    std::vector<CellMapping> getMapping(
        const brain::GIDSet& inputGids) const final;
    brayns::AbstractSimulationHandlerPtr createHandler() const final;

private:
    const std::string _path;
    const float _transitionTime;
    const brain::GIDSet _gids;
    std::shared_ptr<brain::SpikeReportReader> _report;
};
} // namespace bbploader

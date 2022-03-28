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

#pragma once

#include <plugin/io/bbploader/simulation/Simulation.h>

namespace bbploader
{
/**
 * @brief The CompartmentSimulation class implements BBP internal format
 * compartment report handling (soma, full compartment, summation)
 */
class CompartmentSimulation final : public Simulation
{
public:
    /**
     * @brief CompartmentSimulation
     * @param path
     * @param inputGids
     */
    CompartmentSimulation(const std::string &path, const brain::GIDSet &inputGids);

    /**
     * @brief getReportGids
     * @return
     */
    const brain::GIDSet &getReportGids() const override;

    /**
     * @brief getMapping
     * @return
     */
    std::vector<SimulationMapping> getMapping() const override;

    /**
     * @brief addSimulationComponent
     * @param model
     */
    void addSimulationComponent(brayns::Model &model) const override;

private:
    const std::string _path;
    const std::shared_ptr<brion::CompartmentReport> _report;
};
} // namespace bbploader

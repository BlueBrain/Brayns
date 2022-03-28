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

#include <brayns/engine/Model.h>

#include <plugin/io/simulation/SimulationMapping.h>

#include <cstdint>
#include <vector>

#include <brain/brain.h>

namespace bbploader
{
/**
 * @brief The Simulation class is the base class for BBP internal format report
 * types, which allows for abstract access to it, simplifying the loader design
 */
class Simulation
{
public:
    virtual ~Simulation() = default;

    /**
     * @brief return the GIDs that this simulation affects to
     */
    virtual const brain::GIDSet &getReportGids() const = 0;

    /**
     * @brief return the simulation mapping of each cell in the given inputGids
     *        (The simulation mapping is used to compute the offset into a
     * simulation frame, which is a vector of floats)
     */
    virtual std::vector<SimulationMapping> getMapping() const = 0;

    /**
     * @brief creates a brayns::AbstractSimulationHandler object that is capable
     * of handling this type of simulation
     */
    virtual void addSimulationComponent(brayns::Model &model) const = 0;
};
} // namespace bbploader

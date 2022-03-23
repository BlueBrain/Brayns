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

#include <brayns/common/MathTypes.h>

#include <plugin/api/CircuitColorHandler.h>
#include <plugin/io/simulation/SimulationMapping.h>

/**
 * @brief The MorphologyInstance class is the base class to implement
 * representations of a cell geometry, and provides functionality to add
 * simulation mapping based on the cell data.
 */
class MorphologyInstance
{
public:
    virtual ~MorphologyInstance() = default;

    /**
     * @brief transform the simulation mapping data into offsets of the
     * simulation frame array for each piece of geometry that this instance
     * holds.
     * @returns std::vector<uint64_t>
     */
    virtual std::vector<uint64_t> mapSimulation(const SimulationMapping &mapping) const = 0;

    /**
     * @brief adds the geometry in this morphology instance to the model,
     * @param id The id of this instance
     * @param model The model to which to add the geometry
     */
    virtual void addToModel(uint64_t id, brayns::Model &model) = 0;

    /**
     * @brief return the number of segments which makes up the given section.
     * @throws std::runtime_error if the given section does not exists on this
     * instance
     */
    virtual size_t getSectionSegmentCount(const int32_t section) const = 0;

    using SegmentPoints = std::pair<const brayns::Vector3f *, const brayns::Vector3f *>;

    /**
     * @brief return the start and end point of the given segment of the given
     * section
     * @throws std::runtime_error if the given section does not exists on this
     * instance, or if the given segment does not exists on the given section
     */
    virtual SegmentPoints getSegment(const int32_t section, const uint32_t segment) const = 0;
};

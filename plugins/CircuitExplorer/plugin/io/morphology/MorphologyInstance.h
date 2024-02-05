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

#include <brayns/common/MathTypes.h>

#include <plugin/api/CircuitColorHandler.h>

/**
 * @brief The MorphologyInstance class is the base class to implement
 * representations of a cell geometry, and provides functionality to add
 * simulation mapping based on the cell data.
 */
class MorphologyInstance
{
public:
    using Ptr = std::unique_ptr<MorphologyInstance>;

    virtual ~MorphologyInstance() = default;

    /**
     * @brief transform the simulation mapping data into offsets of the
     * simulation frame array for each piece of geometry that this instance
     * holds. The mapping is given as a global offset for this cell, plus local
     *        offsets and number of compartments for each section.
     *        Its assumed that each section is represented by an ID, being the
     * first ID 0
     */
    virtual void mapSimulation(
        const size_t globalOffset, const std::vector<uint16_t>& sectionOffsets,
        const std::vector<uint16_t>& sectionCompartments) = 0;

    /**
     * @brief adds the geometry in this morphology instance to the model,
     * creating the needed materials, and returns a ElementMaterialMap object
     * that allows to access all the materials of the newly added geometry
     */
    virtual ElementMaterialMap::Ptr addToModel(brayns::Model& model) const = 0;

    /**
     * @brief return the number of segments which makes up the given section.
     * @throws std::runtime_error if the given section does not exists on this
     * instance
     */
    virtual size_t getSectionSegmentCount(const int32_t section) const = 0;

    using SegmentPoints =
        std::pair<const brayns::Vector3f*, const brayns::Vector3f*>;

    /**
     * @brief return the start and end point of the given segment of the given
     * section
     * @throws std::runtime_error if the given section does not exists on this
     * instance, or if the given segment does not exists on the given section
     */
    virtual SegmentPoints getSegment(const int32_t section,
                                     const uint32_t segment) const = 0;

    /**
     * @brief return the simulation offset in the geometry associated with the
     * given segment in the given section
     * @throws std::runtime_error if the given section does not exists on this
     * instance, or if the given segment does not exists on the given section
     */
    virtual uint64_t getSegmentSimulationOffset(
        const int32_t section, const uint32_t segment) const = 0;
};

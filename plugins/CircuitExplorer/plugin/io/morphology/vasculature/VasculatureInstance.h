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

#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/morphology/vasculature/VasculatureSection.h>

#include <brayns/engine/geometries/Primitive.h>

/**
 * @brief The VasculatureInstance class represents a vasculature node geometry to be placed on the scene and to which
 * simulation can be mapped. It also gives access to geometry information at the section/segment level to allow for
 * external geometry surface mapping (unused for vasculature)
 */
class VasculatureInstance : public MorphologyInstance
{
public:
    VasculatureInstance(
        const brayns::Vector3f &start,
        const float startR,
        const brayns::Vector3f &end,
        const float endR,
        const VasculatureSection sectionType);

    /**
     * @brief mapSimulation
     * @param mapping
     * @return
     */
    std::vector<uint64_t> mapSimulation(const SimulationMapping &mapping) const final;

    /**
     * @brief addToModel
     * @param id
     * @param model
     */
    void addToModel(uint64_t id, brayns::Model &model) final;

    /**
     * @brief getSectionSegmentCount
     * @return
     */
    size_t getSectionSegmentCount(const int32_t) const final;

    /**
     * @brief getSegment
     * @return
     */
    MorphologyInstance::SegmentPoints getSegment(const int32_t, const uint32_t) const final;

private:
    brayns::Primitive _geometry;
    VasculatureSection _sectionType;
};

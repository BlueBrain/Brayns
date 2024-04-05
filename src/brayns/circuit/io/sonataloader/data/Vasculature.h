/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <bbp/sonata/nodes.h>

#include <brayns/core/utils/MathTypes.h>

#include <brayns/circuit/api/vasculature/VasculatureSection.h>

namespace sonataloader
{
/**
 * @brief Wrapper around bbp::sonata::NodePopulation for vasculature.
 */
class Vasculature
{
public:
    using Nodes = bbp::sonata::NodePopulation;
    using Selection = bbp::sonata::Selection;

    /**
     * @brief returns a list of 3D segment start points for the given node
     * selection
     */
    static std::vector<brayns::Vector3f> getSegmentStartPoints(const Nodes &nodes, const Selection &selection);

    /**
     * @brief returns a list of 3D segment end points for the given node
     * selection
     */
    static std::vector<brayns::Vector3f> getSegmentEndPoints(const Nodes &nodes, const Selection &selection);

    /**
     * @brief returns a list of radii start for the given node selection
     */
    static std::vector<float> getSegmentStartRadii(const Nodes &nodes, const Selection &selection);

    /**
     * @brief returns a list of radii end for the given node selection
     */
    static std::vector<float> getSegmentEndRadii(const Nodes &nodes, const Selection &selection);

    /**
     * @brief returns a list of section types for the given node selection
     */
    static std::vector<VasculatureSection> getSegmentSectionTypes(const Nodes &nodes, const Selection &selection);
};
} // namespace sonataloader

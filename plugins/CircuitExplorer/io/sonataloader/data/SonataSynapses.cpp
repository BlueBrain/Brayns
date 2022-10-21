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

#include "SonataSynapses.h"

#include <algorithm>
#include <map>
#include <random>

namespace
{
constexpr char enumModelType[] = "model_type";

constexpr char attribEffSectionId[] = "efferent_section_id";
constexpr char attribEffSegmentId[] = "efferent_segment_id";
constexpr char attribEffSegmentPos[] = "efferent_section_pos";
constexpr char attribEffSurfPosiX[] = "efferent_surface_x";
constexpr char attribEffSurfPosiY[] = "efferent_surface_y";
constexpr char attribEffSurfPosiZ[] = "efferent_surface_z";

constexpr char attribAffSectionId[] = "afferent_section_id";
constexpr char attribAffSegmentId[] = "afferent_segment_id";
constexpr char attribAffSegmentPos[] = "afferent_section_pos";
constexpr char attribAffSurfPosiX[] = "afferent_surface_x";
constexpr char attribAffSurfPosiY[] = "afferent_surface_y";
constexpr char attribAffSurfPosiZ[] = "afferent_surface_z";

constexpr char attribAstroCenterX[] = "astrocyte_center_x";
constexpr char attribAstroCenterY[] = "astrocyte_center_y";
constexpr char attribAstroCenterZ[] = "astrocyte_center_z";

constexpr char attribEndFeetId[] = "endfoot_id";
constexpr char attribEndFeetSurfPosX[] = "endfoot_surface_x";
constexpr char attribEndFeetSurfPosY[] = "endfoot_surface_y";
constexpr char attribEndFeetSurfPosZ[] = "endfoot_surface_z";

void checkEdgeParameters(const bbp::sonata::EdgePopulation &population, const std::vector<const char *> &inputAttribs)
{
    auto &attribs = population.attributeNames();
    for (auto &attribute : inputAttribs)
    {
        if (attribs.find(attribute) == attribs.end())
        {
            throw std::runtime_error("Edge population " + population.name() + " is missing attribute " + attribute);
        }
    }
}

std::vector<brayns::Vector3f> loadPositions(
    const bbp::sonata::EdgePopulation &pops,
    const bbp::sonata::Selection &selection,
    const char *attribX,
    const char *attribY,
    const char *attribZ)
{
    checkEdgeParameters(pops, {attribX, attribY, attribZ});
    const auto surfaPosXs = pops.getAttribute<float>(attribX, selection);
    const auto surfaPosYs = pops.getAttribute<float>(attribY, selection);
    const auto surfaPosZs = pops.getAttribute<float>(attribZ, selection);

    std::vector<brayns::Vector3f> result(surfaPosXs.size(), brayns::Vector3f(0.f, 0.f, 0.f));
    for (size_t i = 0; i < surfaPosXs.size(); ++i)
    {
        result[i].x = surfaPosXs[i];
        result[i].y = surfaPosYs[i];
        result[i].z = surfaPosZs[i];
    }
    return result;
}
} // namespace

namespace sonataloader
{
std::string SonataSynapses::getPopulationType(const Edges &population)
{
    checkEdgeParameters(population, {enumModelType});
    auto selection = bbp::sonata::Selection::fromValues({0});
    return population.getAttribute<std::string>(enumModelType, selection)[0];
}

std::vector<uint64_t> SonataSynapses::getSourceNodes(const Edges &population, const Selection &selection)
{
    return population.sourceNodeIDs(selection);
}

std::vector<uint64_t> SonataSynapses::getTargetNodes(const Edges &population, const Selection &selection)
{
    return population.targetNodeIDs(selection);
}

std::vector<brayns::Vector3f> SonataSynapses::getAfferentSurfacePos(const Edges &population, const Selection &selection)
{
    return loadPositions(population, selection, attribAffSurfPosiX, attribAffSurfPosiY, attribAffSurfPosiZ);
}

std::vector<brayns::Vector3f> SonataSynapses::getEfferentSurfacePos(const Edges &population, const Selection &selection)
{
    return loadPositions(population, selection, attribEffSurfPosiX, attribEffSurfPosiY, attribEffSurfPosiZ);
}

std::vector<brayns::Vector3f> SonataSynapses::getEfferentAstrocyteCenterPos(
    const Edges &population,
    const Selection &selection)
{
    return loadPositions(population, selection, attribAstroCenterX, attribAstroCenterY, attribAstroCenterZ);
}

std::vector<uint64_t> SonataSynapses::getEndFeetIds(const Edges &population, const Selection &selection)
{
    checkEdgeParameters(population, {attribEndFeetId});
    return population.getAttribute<uint64_t>(attribEndFeetId, selection);
}
} // namespace sonataloader

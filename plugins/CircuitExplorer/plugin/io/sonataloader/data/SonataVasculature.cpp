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

#include "SonataVasculature.h"

#include <algorithm>

namespace sonataloader
{
namespace
{
constexpr char attribStartX[] = "start_x";
constexpr char attribStartY[] = "start_y";
constexpr char attribStartZ[] = "start_z";
constexpr char attribEndX[] = "end_x";
constexpr char attribEndY[] = "end_y";
constexpr char attribEndZ[] = "end_z";
constexpr char attribStartDiameter[] = "start_diameter";
constexpr char attribEndDiameter[] = "end_diameter";
constexpr char attribStartNode[] = "start_node";
constexpr char attribEndNode[] = "end_node";
constexpr char attribType[] = "type";
constexpr char attribSectionId[] = "section_id";

void checkAttributes(const bbp::sonata::NodePopulation& nodes,
                     const std::vector<const char*>& attribs)
{
    const auto& attributes = nodes.attributeNames();
    for (const auto attrib : attribs)
    {
        if (attributes.find(attrib) == attributes.end())
        {
            throw std::runtime_error("Node population '" + nodes.name() +
                                     "' is missing "
                                     "attribute " +
                                     attrib);
        }
    }
}

std::vector<brayns::Vector3f> getPoints(
    const bbp::sonata::NodePopulation& nodes,
    const bbp::sonata::Selection& selection, const char* ax, const char* ay,
    const char* az)
{
    checkAttributes(nodes, {ax, ay, az});
    const auto x = nodes.getAttribute<float>(ax, selection);
    const auto y = nodes.getAttribute<float>(ay, selection);
    const auto z = nodes.getAttribute<float>(az, selection);

    std::vector<brayns::Vector3f> result(x.size());
    for (size_t i = 0; i < x.size(); ++i)
    {
        result[i].x = x[i];
        result[i].y = y[i];
        result[i].z = z[i];
    }
    return result;
}

std::vector<float> getRadii(const bbp::sonata::NodePopulation& nodes,
                            const bbp::sonata::Selection& selection,
                            const char* ar)
{
    checkAttributes(nodes, {ar});
    auto radii = nodes.getAttribute<float>(ar, selection);
    std::transform(radii.begin(), radii.end(), radii.begin(),
                   [](const float diameter) { return diameter * .5f; });
    return radii;
}
} // namespace

std::vector<brayns::Vector3f> SonataVasculature::getSegmentStartPoints(
    const Nodes& nodes, const Selection& selection)
{
    return getPoints(nodes, selection, attribStartX, attribStartY,
                     attribStartZ);
}

std::vector<brayns::Vector3f> SonataVasculature::getSegmentEndPoints(
    const Nodes& nodes, const Selection& selection)
{
    return getPoints(nodes, selection, attribEndX, attribEndY, attribEndZ);
}

std::vector<float> SonataVasculature::getSegmentStartRadii(
    const Nodes& nodes, const Selection& selection)
{
    return getRadii(nodes, selection, attribStartDiameter);
}

std::vector<float> SonataVasculature::getSegmentEndRadii(
    const Nodes& nodes, const Selection& selection)
{
    return getRadii(nodes, selection, attribEndDiameter);
}

std::vector<VasculatureSection> SonataVasculature::getSegmentSectionTypes(
    const Nodes& nodes, const Selection& selection)
{
    checkAttributes(nodes, {attribType});
    const auto rawTypes = nodes.getAttribute<uint8_t>(attribType, selection);
    std::vector<VasculatureSection> result(rawTypes.size());
    for (size_t i = 0; i < rawTypes.size(); ++i)
        result[i] = static_cast<VasculatureSection>(rawTypes[i]);
    return result;
}
} // namespace sonataloader

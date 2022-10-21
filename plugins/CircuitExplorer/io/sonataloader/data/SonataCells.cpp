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

#include "SonataCells.h"

#include <bbp/sonata/node_sets.h>

namespace sonataloader
{
namespace
{
constexpr char enumModelType[] = "model_type";

constexpr char attribX[] = "x";
constexpr char attribY[] = "y";
constexpr char attribZ[] = "z";
constexpr char attribOrientationX[] = "orientation_x";
constexpr char attribOrientationY[] = "orientation_y";
constexpr char attribOrientationZ[] = "orientation_z";
constexpr char attribOrientationW[] = "orientation_w";
constexpr char attribLayer[] = "layer";
constexpr char attribRegion[] = "region";
constexpr char attribMtype[] = "mtype";
constexpr char attribEtype[] = "etype";
constexpr char attribMorphology[] = "morphology";

std::vector<std::string> getEnumValueList(
    const bbp::sonata::NodePopulation &population,
    const bbp::sonata::Selection &selection,
    const std::string &attribute)
{
    auto enumValues = population.enumerationValues(attribute);
    auto enumIndices = population.getEnumeration<size_t>(attribute, selection);

    std::vector<std::string> result(enumIndices.size());
    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = enumValues[enumIndices[i]];
    }
    return result;
}

void checkAttributes(const bbp::sonata::NodePopulation &nodes, const std::vector<const char *> &attribs)
{
    auto &attributes = nodes.attributeNames();
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

} // namespace

std::string SonataCells::getPopulationType(const Nodes &nodes)
{
    checkAttributes(nodes, {enumModelType});
    auto selection = bbp::sonata::Selection::fromValues({0});
    return nodes.getAttribute<std::string>(enumModelType, selection)[0];
}

std::vector<std::string> SonataCells::getMorphologies(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribMorphology});
    return nodes.getAttribute<std::string>(attribMorphology, selection);
}

std::vector<brayns::Vector3f> SonataCells::getPositions(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribX, attribY, attribZ});

    auto xPos = nodes.getAttribute<float>(attribX, selection);
    auto yPos = nodes.getAttribute<float>(attribY, selection);
    auto zPos = nodes.getAttribute<float>(attribZ, selection);

    std::vector<brayns::Vector3f> result(xPos.size());
    for (size_t i = 0; i < xPos.size(); ++i)
    {
        result[i].x = xPos[i];
        result[i].y = yPos[i];
        result[i].z = zPos[i];
    }
    return result;
}

std::vector<brayns::Quaternion> SonataCells::getRotations(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribOrientationW, attribOrientationX, attribOrientationY, attribOrientationZ});

    auto x = nodes.getAttribute<float>(attribOrientationX, selection);
    auto y = nodes.getAttribute<float>(attribOrientationY, selection);
    auto z = nodes.getAttribute<float>(attribOrientationZ, selection);
    auto w = nodes.getAttribute<float>(attribOrientationW, selection);

    std::vector<brayns::Quaternion> result(x.size());
#pragma omp parallel for
    for (size_t i = 0; i < x.size(); ++i)
    {
        result[i] = glm::normalize(brayns::Quaternion(w[i], x[i], y[i], z[i]));
    }

    return result;
}

std::vector<std::string> SonataCells::getLayers(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribLayer});
    return nodes.getAttribute<std::string>(attribLayer, selection);
}

std::vector<std::string> SonataCells::getRegions(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribRegion});
    return getEnumValueList(nodes, selection, attribRegion);
}

std::vector<std::string> SonataCells::getMTypes(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribMtype});
    return getEnumValueList(nodes, selection, attribMtype);
}

std::vector<std::string> SonataCells::getETypes(const Nodes &nodes, const Selection &selection)
{
    checkAttributes(nodes, {attribEtype});
    return getEnumValueList(nodes, selection, attribEtype);
}
} // namespace sonataloader

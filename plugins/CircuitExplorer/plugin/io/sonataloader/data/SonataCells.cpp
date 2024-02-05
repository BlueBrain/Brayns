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

#include "SonataCells.h"

#include <bbp/sonata/node_sets.h>

#include <glm/gtx/matrix_decompose.hpp>

#include <brayns/common/Log.h>

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
    const bbp::sonata::NodePopulation& population,
    const bbp::sonata::Selection& selection, const std::string& attribute)
{
    const auto enumValues = population.enumerationValues(attribute);
    const auto enumIndices =
        population.getEnumeration<size_t>(attribute, selection);

    std::vector<std::string> result(enumIndices.size());
    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = enumValues[enumIndices[i]];
    }
    return result;
}

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

void checkEnums(const bbp::sonata::NodePopulation& nodes,
                const std::vector<const char*>& enums)
{
    const auto& enumerations = nodes.enumerationNames();
    for (const auto enumName : enums)
    {
        if (enumerations.find(enumName) == enumerations.end())
        {
            throw std::runtime_error("Node population '" + nodes.name() +
                                     "' is missing "
                                     "enumeration " +
                                     enumName);
        }
    }
}

} // namespace

std::string SonataCells::getPopulationType(const Nodes& nodes)
{
    // Standard still not stable on this point. Not sure if it will
    // be an enum on the library group. or a dataset of the population
    // itself.
    // Test data has it as dataset of the population

    brayns::Log::warn(
        "[CE] SonataCells::getPopulationType(): Extracted from population "
        "dataset.");

    // Library enum
    // checkEnums(nodes, {enumModelType});
    // return nodes.enumerationValues(enumModelType)[0];

    // Population dataset
    checkAttributes(nodes, {enumModelType});
    // Select the first node only
    const auto selection = bbp::sonata::Selection::fromValues({0});
    return nodes.getAttribute<std::string>(enumModelType, selection)[0];
}

std::vector<std::string> SonataCells::getMorphologies(
    const Nodes& nodes, const Selection& selection)
{
    checkAttributes(nodes, {attribMorphology});
    return nodes.getAttribute<std::string>(attribMorphology, selection);
}

std::vector<brayns::Vector3f> SonataCells::getPositions(
    const Nodes& nodes, const Selection& selection)
{
    checkAttributes(nodes, {attribX, attribY, attribZ});

    const auto xPos = nodes.getAttribute<float>(attribX, selection);
    const auto yPos = nodes.getAttribute<float>(attribY, selection);
    const auto zPos = nodes.getAttribute<float>(attribZ, selection);

    std::vector<brayns::Vector3f> result(xPos.size());
    //#pragma omp parallel for
    for (size_t i = 0; i < xPos.size(); ++i)
    {
        result[i].x = xPos[i];
        result[i].y = yPos[i];
        result[i].z = zPos[i];
    }
    return result;
}

std::vector<brayns::Quaternion> SonataCells::getRotations(
    const Nodes& nodes, const Selection& selection)
{
    checkAttributes(nodes, {attribOrientationW, attribOrientationX,
                            attribOrientationY, attribOrientationZ});

    const auto x = nodes.getAttribute<float>(attribOrientationX, selection);
    const auto y = nodes.getAttribute<float>(attribOrientationY, selection);
    const auto z = nodes.getAttribute<float>(attribOrientationZ, selection);
    const auto w = nodes.getAttribute<float>(attribOrientationW, selection);

    std::vector<brayns::Quaternion> result(x.size());
    //#pragma omp parallel for
    for (size_t i = 0; i < x.size(); ++i)
        result[i] = glm::normalize(brayns::Quaternion(w[i], x[i], y[i], z[i]));

    return result;
}

std::vector<std::string> SonataCells::getLayers(const Nodes& nodes,
                                                const Selection& selection)
{
    checkAttributes(nodes, {attribLayer});
    return nodes.getAttribute<std::string>(attribLayer, selection);
}

std::vector<std::string> SonataCells::getRegions(const Nodes& nodes,
                                                 const Selection& selection)
{
    checkAttributes(nodes, {attribRegion});
    return getEnumValueList(nodes, selection, attribRegion);
}

std::vector<std::string> SonataCells::getMTypes(const Nodes& nodes,
                                                const Selection& selection)
{
    checkAttributes(nodes, {attribMtype});
    return getEnumValueList(nodes, selection, attribMtype);
}

std::vector<std::string> SonataCells::getETypes(const Nodes& nodes,
                                                const Selection& selection)
{
    checkAttributes(nodes, {attribEtype});
    return getEnumValueList(nodes, selection, attribEtype);
}
} // namespace sonataloader

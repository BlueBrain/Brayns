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

#include "CommonEdgeColorData.h"

#include <brayns/common/Log.h>

#include <plugin/io/sonataloader/data/SonataSynapses.h>

#include <bbp/sonata/config.h>

#include <algorithm>
#include <set>

namespace sonataloader
{
namespace
{
constexpr char methodByMorphology[] = "connected morphology";
constexpr char methodByLayer[] = "connected layer";
constexpr char methodByMorphClass[] = "connected morph_class";
constexpr char methodByEType[] = "connected etype";
constexpr char methodByMType[] = "connected mtype";
constexpr char methodBySynapseClass[] = "connected synapse_class";
constexpr char methodByRegion[] = "connected region";
constexpr char methodByHemisphere[] = "connected hemisphere";

constexpr char attribMorphology[] = "morphology";
constexpr char attribLayer[] = "layer";
constexpr char attribMorphClass[] = "morph_class";
constexpr char attribEType[] = "etype";
constexpr char attribMType[] = "mtype";
constexpr char attribSynapseClass[] = "synapse_class";
constexpr char attribRegion[] = "region";
constexpr char attribHemisphere[] = "hemisphere";

std::string
    getNodePopulation(const bbp::sonata::CircuitConfig &config, const std::string &edgePopulation, const bool afferent)
{
    const auto edges = config.getEdgePopulation(edgePopulation);
    return afferent ? edges.target() : edges.source();
}

std::vector<std::pair<const char *, const char *>> methodAttribMapping() noexcept
{
    const std::vector<std::pair<const char *, const char *>> possibleMethods = {
        {methodByMorphology, attribMorphology},
        {methodByLayer, attribLayer},
        {methodByMorphClass, attribMorphClass},
        {methodByEType, attribEType},
        {methodByMType, attribMType},
        {methodBySynapseClass, attribSynapseClass},
        {methodByRegion, attribRegion},
        {methodByHemisphere, attribHemisphere}};
    return possibleMethods;
}

const char *getAttribForMethod(const std::string &method)
{
    const auto &mapping = methodAttribMapping();
    for (const auto &entry : mapping)
    {
        if (method == entry.first)
        {
            return entry.second;
        }
    }

    throw std::invalid_argument("CommonEdgeColorData: Unknown method '" + method + "'");
}

std::vector<std::string> getMethodsValues(
    const bbp::sonata::CircuitConfig &config,
    const std::string &nodePopulation,
    const std::string &method,
    const std::vector<uint64_t> &nodeIds)
{
    const auto attrib = getAttribForMethod(method);
    try
    {
        const std::set<uint64_t> uniqueNodes(nodeIds.begin(), nodeIds.end());
        const auto nodes = config.getNodePopulation(nodePopulation);
        const auto nodeSelection = bbp::sonata::Selection::fromValues(uniqueNodes.begin(), uniqueNodes.end());
        return nodes.getAttribute<std::string>(attrib, nodeSelection);
    }
    catch (const std::exception &e)
    {
        brayns::Log::warn(
            "Could not retrieve data from population {}, attribute {}: {}",
            nodePopulation,
            attrib,
            e.what());
    }

    return {};
}
} // namespace

CommonEdgeColorData::CommonEdgeColorData(
    bbp::sonata::CircuitConfig config,
    const std::string &edgePopulation,
    const bool afferent)
    : _config(std::move(config))
    , _edgePopulation(edgePopulation)
    , _nodePopulation(getNodePopulation(_config, _edgePopulation, afferent))
    , _afferent(afferent)
{
}

std::vector<std::string> CommonEdgeColorData::getMethods() const noexcept
{
    const auto nodes = _config.getNodePopulation(_nodePopulation);
    const auto &attributes = nodes.attributeNames();
    const auto &possibleMethods = methodAttribMapping();

    std::vector<std::string> result;
    result.reserve(possibleMethods.size());
    for (const auto &possibleMethod : possibleMethods)
    {
        if (attributes.find(possibleMethod.second) != attributes.end())
        {
            // Check we can actually retrieve the data
            const auto &methodName = possibleMethod.first;
            const std::vector<uint64_t> dummySelection = {0};
            const auto variableTest = getMethodsValues(_config, _nodePopulation, methodName, dummySelection);
            if (!variableTest.empty())
                result.emplace_back(possibleMethod.first);
        }
    }

    result.shrink_to_fit();
    return result;
}

std::vector<std::string> CommonEdgeColorData::getMethodVariables(const std::string &method) const
{
    const auto nodePopulation = _config.getNodePopulation(_nodePopulation);
    const auto all = nodePopulation.selectAll();
    const auto allFlat = all.flatten();
    const auto values = getMethodsValues(_config, _nodePopulation, method, allFlat);
    const std::set<std::string> unique(values.begin(), values.end());
    return std::vector<std::string>(unique.begin(), unique.end());
}

std::vector<std::string> CommonEdgeColorData::getMethodValuesForIDs(
        const std::string &method, const std::vector<uint64_t> &ids) const
{
    return getMethodsValues(_config, _nodePopulation, method, ids);
}
} // namespace sonataloader

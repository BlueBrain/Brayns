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

#include "CommonEdgeColorHandler.h"

#include <brayns/engine/Material.h>

#include <bbp/sonata/config.h>

#include <plugin/api/ColorUtils.h>
#include <plugin/io/sonataloader/data/SonataSynapses.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

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

std::string getNodePopulation(const bbp::sonata::CircuitConfig& config,
                              const std::string& edgePopulation,
                              const bool afferent)
{
    const auto edges = config.getEdgePopulation(edgePopulation);
    return afferent ? edges.target() : edges.source();
}

std::vector<std::pair<const char*, const char*>> methodAttribMapping() noexcept
{
    const std::vector<std::pair<const char*, const char*>> possibleMethods = {
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

const char* getAttribForMethod(const std::string& method)
{
    const auto& mapping = methodAttribMapping();
    for (const auto& entry : mapping)
    {
        if (method == entry.first)
            return entry.second;
    }

    throw std::invalid_argument("CommonEdgeColorHandler: Unknown method '" +
                                method + "'");
}

std::vector<std::string> getMethodsValues(
    const bbp::sonata::CircuitConfig& config, const std::string& nodePopulation,
    const std::string& method, const std::vector<uint64_t>& nodeIds)
{
    const std::set<uint64_t> uniqueNodes(nodeIds.begin(), nodeIds.end());
    const auto nodes = config.getNodePopulation(nodePopulation);
    const auto attrib = getAttribForMethod(method);
    const auto nodeSelection =
        bbp::sonata::Selection::fromValues(uniqueNodes.begin(),
                                           uniqueNodes.end());
    return nodes.getAttribute<std::string>(attrib, nodeSelection);
}
} // namespace

CommonEdgeColorHandler::CommonEdgeColorHandler(
    const std::string& configPath, const std::string& edgePopulation,
    const bool afferent)
    : _config(bbp::sonata::CircuitConfig::fromFile(configPath))
    , _edgePopulation(edgePopulation)
    , _nodePopulation(getNodePopulation(_config, _edgePopulation, afferent))
    , _afferent(afferent)
{
}

void CommonEdgeColorHandler::_setElementsImpl(
    const std::vector<uint64_t>&,
    std::vector<ElementMaterialMap::Ptr>&& elements)
{
    // We get the edges grouped by the node to which they belong.
    // We arrange so that they are mapped to the node id they connect to instead
    // We cant use edges.afferentEdges(ids) or edges.efferentEdges(ids) because
    // we dont know which edges has been added to the geometry, nor the
    // percentage used.

    // Flatten edges
    std::vector<SynapseMaterialInfo> tempBuf;
    for (auto& element : elements)
    {
        auto& semm = static_cast<SynapseMaterialMap&>(*element.get());
        tempBuf.insert(tempBuf.end(), semm.materials.begin(),
                       semm.materials.end());
    }

    std::sort(tempBuf.begin(), tempBuf.end(),
              [](const SynapseMaterialInfo& a, const SynapseMaterialInfo& b) {
                  return a.id < b.id;
              });

    // Get edge ids to query the node ids of each
    std::vector<uint64_t> edgeIds;
    edgeIds.reserve(tempBuf.size());
    for (const auto& edge : tempBuf)
        edgeIds.push_back(edge.id);

    // Get opposite node Ids depending wether its afferent or efferent loaded
    // population
    const auto selection = bbp::sonata::Selection::fromValues(edgeIds);
    const auto edges = _config.getEdgePopulation(_edgePopulation);
    const auto nodeIds = _afferent
                             ? SonataSynapses::getSourceNodes(edges, selection)
                             : SonataSynapses::getTargetNodes(edges, selection);

    // Group them
    std::map<uint64_t, std::vector<SynapseMaterialInfo>> groupedEdges;
    for (size_t i = 0; i < nodeIds.size(); ++i)
        groupedEdges[nodeIds[i]].push_back(std::move(tempBuf[i]));

    // Flatten the groupping
    _nodeIds.resize(groupedEdges.size());
    _elements.resize(groupedEdges.size());
    auto groupedIt = groupedEdges.begin();
    for (size_t i = 0; i < _nodeIds.size(); ++i, ++groupedIt)
    {
        _nodeIds[i] = groupedIt->first;
        _elements[i].reserve(groupedIt->second.size());
        for (const auto& edge : groupedIt->second)
            _elements[i].push_back(edge.material);
    }
}

std::vector<std::string> CommonEdgeColorHandler::_getMethodsImpl() const
{
    const auto nodes = _config.getNodePopulation(_nodePopulation);
    const auto& attributes = nodes.attributeNames();
    const auto& possibleMethods = methodAttribMapping();

    std::vector<std::string> result;
    result.reserve(possibleMethods.size());
    for (const auto& possibleMethod : possibleMethods)
    {
        if (attributes.find(possibleMethod.second) != attributes.end())
            result.emplace_back(possibleMethod.first);
    }

    result.shrink_to_fit();
    return result;
}

std::vector<std::string> CommonEdgeColorHandler::_getMethodVariablesImpl(
    const std::string& method) const
{
    const auto values =
        getMethodsValues(_config, _nodePopulation, method, _nodeIds);
    const std::set<std::string> unique(values.begin(), values.end());
    return std::vector<std::string>(unique.begin(), unique.end());
}

void CommonEdgeColorHandler::_updateColorByIdImpl(
    const std::map<uint64_t, brayns::Vector4f>& colorMap)
{
    ColorRoulette cr;
    for (const auto& group : _elements)
    {
        for (const auto matId : group)
            _updateMaterial(matId, cr.getNextColor());
    }
}

void CommonEdgeColorHandler::_updateSingleColorImpl(
    const brayns::Vector4f& color)
{
    for (const auto& group : _elements)
    {
        for (const auto matId : group)
            _updateMaterial(matId, color);
    }
}

void CommonEdgeColorHandler::_updateColorImpl(
    const std::string& method,
    const std::vector<ColoringInformation>& variables)
{
    const auto values =
        getMethodsValues(_config, _nodePopulation, method, _nodeIds);

    if (!variables.empty())
    {
        // Map each attribute to the list indices it will affect
        std::unordered_map<std::string, std::vector<size_t>> valueNodeMap;
        for (size_t i = 0; i < values.size(); ++i)
            valueNodeMap[values[i]].push_back(i);

        // Update only the materials specified in the variables
        for (const auto& entry : variables)
        {
            auto indexIt = valueNodeMap.find(entry.variable);
            if (indexIt != valueNodeMap.end())
            {
                for (const auto index : indexIt->second)
                {
                    for (const auto& material : _elements[index])
                        _updateMaterial(material, entry.color);
                }
            }
        }
    }
    else
    {
        ColorDeck deck;
        for (size_t i = 0; i < values.size(); ++i)
        {
            const auto& color = deck.getColorForKey(values[i]);
            for (const auto matId : _elements[i])
                _updateMaterial(matId, color);
        }
    }
}
} // namespace sonataloader

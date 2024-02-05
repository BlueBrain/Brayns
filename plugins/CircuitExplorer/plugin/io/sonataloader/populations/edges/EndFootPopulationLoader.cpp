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

#include "EndFootPopulationLoader.h"

#include <brayns/json/Json.h>

#include <brayns/utils/Filesystem.h>

#include <plugin/io/sonataloader/data/SonataEndFeetReader.h>
#include <plugin/io/sonataloader/data/SonataSelection.h>
#include <plugin/io/sonataloader/data/SonataSynapses.h>
#include <plugin/io/synapse/groups/EndFootGroup.h>

namespace sonataloader
{
namespace
{
// The endfeet mesh file is not retruned by bbp::sonata::CircuitConfig (by now).
// Get it manually from the expanded json
std::string getEndFeetAreasPath(const bbp::sonata::CircuitConfig& config,
                                const std::string& edgePopulation,
                                const std::string& basePath)
{
    auto parsedJson = brayns::Json::parse(config.getExpandedJSON());
    const auto json = parsedJson.extract<brayns::JsonObject::Ptr>();

    std::string resultPath = "";

    // First fetch default one, if any
    if (const auto components = json->getObject("components"))
    {
        if (components->has("end_feet_area"))
            resultPath =
                components->get("end_feet_area").extract<std::string>();
    }

    const auto edgeNetworkList = json->getObject("networks")->getArray("edges");
    bool found = false;
    for (const auto entry : *edgeNetworkList)
    {
        if (found)
            break;

        const auto& entryObject = entry.extract<Poco::JSON::Object::Ptr>();
        auto edgeFile = entryObject->get("edges_file").extract<std::string>();

        if (!fs::path(edgeFile).is_absolute())
            edgeFile =
                fs::absolute(fs::path(basePath) / fs::path(edgeFile)).string();

        const auto populationStorage = bbp::sonata::EdgeStorage(edgeFile);

        for (const auto& population : populationStorage.populationNames())
        {
            if (population != edgePopulation)
                continue;

            found = true;

            if (const auto popObject = entryObject->getObject("populations"))
            {
                if (const auto edgePopObject =
                        popObject->getObject(edgePopulation))
                {
                    if (edgePopObject->has("end_feet_area"))
                        resultPath = edgePopObject->get("end_feet_area")
                                         .extract<std::string>();
                }
            }
            break;
        }
    }

    if (resultPath.empty())
        throw std::runtime_error(
            "EndFootPopulationLoader: Cannot locate endfeet areas H5 file");
    else if (!fs::path(resultPath).is_absolute())
        resultPath = fs::path(fs::path(basePath) / fs::path(resultPath))
                         .lexically_normal()
                         .string();

    return resultPath;
}
} // namespace

std::vector<std::unique_ptr<SynapseGroup>> EndFootPopulationLoader::load(
    const SonataConfig::Data& networkData,
    const SonataEdgePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection) const
{
    if (lc.load_afferent)
        throw std::runtime_error(
            "Afferent edges not supported on endfoot connectivity");

    const auto basePath = fs::path(networkData.path).parent_path().string();
    auto path =
        getEndFeetAreasPath(networkData.config, lc.edge_population, basePath);

    const auto nodes = nodeSelection.flatten();
    const auto population =
        networkData.config.getEdgePopulation(lc.edge_population);

    const auto edgeSelection = EdgeSelection(population.efferentEdges(nodes))
                                   .intersection(lc.edge_percentage);
    const auto flatEdges = edgeSelection.flatten();
    const auto sourceNodes =
        SonataSynapses::getSourceNodes(population, edgeSelection);
    const auto endFeetIds =
        SonataSynapses::getEndFeetIds(population, edgeSelection);
    const auto endFeetPos =
        SonataSynapses::getEndFeetSurfacePos(population, edgeSelection);

    auto meshes =
        SonataEndFeetReader::readEndFeet(path, endFeetIds, endFeetPos);

    // Initialize for every node, so the flat result will have a group for every
    // node (even if its empty, which allows to simply use vectors)
    std::map<uint64_t, std::unique_ptr<SynapseGroup>> mapping;
    for (const auto nodeId : nodes)
        mapping[nodeId] = std::make_unique<EndFootGroup>();

    // Group endfeet by the node id they belong to
    for (size_t i = 0; i < endFeetIds.size(); ++i)
    {
        EndFootGroup& group =
            static_cast<EndFootGroup&>(*mapping[sourceNodes[i]].get());
        group.addSynapse(endFeetIds[i], std::move(meshes[i]));
    }

    // Flatten
    std::vector<std::unique_ptr<SynapseGroup>> result(nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i)
        result[i] = std::move(mapping[nodes[i]]);

    return result;
}
} // namespace sonataloader

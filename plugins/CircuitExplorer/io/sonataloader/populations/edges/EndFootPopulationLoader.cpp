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

#include "EndFootPopulationLoader.h"

#include <filesystem>

#include <brayns/json/Json.h>

#include <api/synapse/EndfeetColorHandler.h>
#include <components/CircuitColorComponent.h>
#include <components/EndfeetComponent.h>
#include <io/sonataloader/colordata/edge/CommonEdgeColorData.h>
#include <io/sonataloader/data/SonataEndFeetReader.h>
#include <io/sonataloader/data/SonataSynapses.h>

namespace
{
namespace sl = sonataloader;
struct EndFeetAreasPathResolver
{
    // The endfeet mesh file is not retruned by bbp::sonata::CircuitConfig (by now).
    // Get it manually from the expanded json
    static std::string resolve(const sl::EdgeLoadContext &context)
    {
        const auto &network = context.config;
        const auto &config = network.circuitConfig();
        const auto basePath = std::filesystem::path(network.circuitConfigDir());
        auto parsedJson = brayns::Json::parse(config.getExpandedJSON());
        const auto json = parsedJson.extract<brayns::JsonObject::Ptr>();
        const auto &population = context.edgePopulation;
        const auto edgeName = population.name();

        std::string resultPath = "";

        // First fetch default one, if any
        if (const auto components = json->getObject("components"))
        {
            if (components->has("end_feet_area"))
                resultPath = components->get("end_feet_area").extract<std::string>();
        }

        const auto edgeNetworkList = json->getObject("networks")->getArray("edges");
        bool found = false;
        for (const auto &entry : *edgeNetworkList)
        {
            if (found)
            {
                break;
            }

            const auto &entryObject = entry.extract<Poco::JSON::Object::Ptr>();
            auto edgeFile = entryObject->get("edges_file").extract<std::string>();

            if (!std::filesystem::path(edgeFile).is_absolute())
            {
                const auto edgeFileSubpath = std::filesystem::path(edgeFile);
                const auto edgeFileFullPath = std::filesystem::path(basePath) / std::filesystem::path(edgeFile);
                edgeFile = edgeFileFullPath.string();
            }

            const auto populationStorage = bbp::sonata::EdgeStorage(edgeFile);
            for (const auto &population : populationStorage.populationNames())
            {
                if (population != edgeName)
                {
                    continue;
                }

                found = true;

                if (const auto popObject = entryObject->getObject("populations"))
                {
                    if (const auto edgePopObject = popObject->getObject(edgeName))
                    {
                        if (edgePopObject->has("endfeet_meshes"))
                        {
                            resultPath = edgePopObject->get("endfeet_meshes").extract<std::string>();
                        }

                        if (edgePopObject->has("endfeet_meshes_file"))
                        {
                            resultPath = edgePopObject->get("endfeet_meshes_file").extract<std::string>();
                        }
                    }
                }
                break;
            }
        }

        if (resultPath.empty())
        {
            throw std::runtime_error("EndFootPopulationLoader: Cannot locate endfeet areas H5 file");
        }

        if (!std::filesystem::path(resultPath).is_absolute())
        {
            const auto resultSubpath = std::filesystem::path(resultPath);
            auto fullPath = std::filesystem::path(basePath) / resultSubpath;
            fullPath = fullPath.lexically_normal();
            resultPath = fullPath.string();
        }

        return resultPath;
    }
};
} // namespace

namespace sonataloader
{
std::string EndFootPopulationLoader::getPopulationType() const noexcept
{
    return "endfoot";
}

void EndFootPopulationLoader::load(EdgeLoadContext &context) const
{
    const auto path = EndFeetAreasPathResolver::resolve(context);

    const auto &nodeSelection = context.nodeSelection;
    const auto nodes = nodeSelection.flatten();
    const auto &population = context.edgePopulation;
    const auto &edgeSelection = context.edgeSelection;
    const auto flatEdges = edgeSelection.flatten();
    const auto astrocyteIds = SonataSynapses::getTargetNodes(population, edgeSelection);
    const auto endFeetIds = SonataSynapses::getEndFeetIds(population, edgeSelection);

    auto meshes = SonataEndFeetReader::readEndFeet(path, endFeetIds);

    std::map<uint64_t, std::vector<brayns::TriangleMesh>> endfeetGeometry;
    for (size_t i = 0; i < astrocyteIds.size(); ++i)
    {
        const auto astrocyte = astrocyteIds[i];
        auto &buffer = endfeetGeometry[astrocyte];
        auto &mesh = meshes[i];
        buffer.push_back(std::move(mesh));
    }

    auto &model = context.model;

    // Geometry
    auto &endfeet = model.addComponent<EndfeetComponent>();
    endfeet.addEndfeet(endfeetGeometry);

    // Coloring
    const auto &network = context.config;
    const auto &config = network.circuitConfig();
    const auto astrocytePopulationName = population.target();
    auto astrocytePopulation = config.getNodePopulation(astrocytePopulationName);
    auto colorData = std::make_unique<CommonEdgeColorData>(std::move(astrocytePopulation));
    auto colorHandler = std::make_unique<EndfeetColorHandler>(endfeet);
    model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));
}
} // namespace sonataloader

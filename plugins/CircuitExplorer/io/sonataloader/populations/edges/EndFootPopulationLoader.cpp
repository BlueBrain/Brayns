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

#include <brayns/json/Json.h>

#include <api/circuit/EndfeetCircuitBuilder.h>
#include <io/sonataloader/colordata/ColorDataFactory.h>
#include <io/sonataloader/data/EndFeetReader.h>
#include <io/sonataloader/data/Names.h>
#include <io/sonataloader/data/Synapses.h>

#include <filesystem>

namespace
{
class EndFeetAreasPath
{
public:
    EndFeetAreasPath(const sonataloader::EdgeLoadContext &context)
        : _json(_parseConfig(context))
        , _rootPath(_getConfigRootPath(context))
        , _edgeName(context.edgePopulation.name())
    {
    }

    std::string resolve()
    {
        auto resultPath = _findEndfeetPath();
        if (resultPath.empty())
        {
            resultPath = _getDefaultPath();
        }

        if (resultPath.empty())
        {
            throw std::runtime_error("EndFootPopulationLoader: Cannot locate endfeet areas H5 file");
        }

        if (!std::filesystem::path(resultPath).is_absolute())
        {
            auto fullPath = _rootPath / std::filesystem::path(resultPath);
            resultPath = fullPath.lexically_normal().string();
        }

        return resultPath;
    }

private:
    Poco::JSON::Object::Ptr _parseConfig(const sonataloader::EdgeLoadContext &context)
    {
        auto &config = context.config;
        auto parsedJson = brayns::Json::parse(config.getConfigAsJson());
        return parsedJson.extract<brayns::JsonObject::Ptr>();
    }

    std::filesystem::path _getConfigRootPath(const sonataloader::EdgeLoadContext &context)
    {
        auto &config = context.config;
        return std::filesystem::path(config.getBasePath());
    }

    std::string _getDefaultPath()
    {
        // First fetch default one, if any
        if (const auto components = _json->getObject("components"))
        {
            if (components->has("end_feet_area"))
            {
                return components->get("end_feet_area").extract<std::string>();
            }
        }
        return {};
    }

    Poco::JSON::Object::Ptr _findPopulationObject()
    {
        auto edgeNetworks = _json->getObject("networks")->getArray("edges");
        for (auto &entry : *edgeNetworks)
        {
            auto &entryObject = entry.extract<Poco::JSON::Object::Ptr>();
            auto edgeFile = _getPathToEdgeFile(entryObject);

            auto populationStorage = bbp::sonata::EdgeStorage(edgeFile);
            for (auto &population : populationStorage.populationNames())
            {
                if (population != _edgeName)
                {
                    continue;
                }
                auto popObject = entryObject->getObject("populations");
                if (!popObject)
                {
                    return nullptr;
                }
                return popObject->getObject(_edgeName);
            }
        }
        return nullptr;
    }

    std::string _findEndfeetPath()
    {
        auto json = _findPopulationObject();

        if (json->has("endfeet_meshes"))
        {
            return json->get("endfeet_meshes").extract<std::string>();
        }
        if (json->has("endfeet_meshes_file"))
        {
            return json->get("endfeet_meshes_file").extract<std::string>();
        }
        return {};
    }

    std::string _getPathToEdgeFile(const Poco::JSON::Object::Ptr &json)
    {
        auto edgeFile = json->get("edges_file").extract<std::string>();
        auto edgeFilePath = std::filesystem::path(edgeFile);
        if (!edgeFilePath.is_absolute())
        {
            auto edgeFileFullPath = _rootPath / edgeFilePath;
            edgeFile = edgeFileFullPath.string();
        }
        return edgeFile;
    }

private:
    Poco::JSON::Object::Ptr _json;
    std::filesystem::path _rootPath;
    std::string _edgeName;
};

} // namespace

namespace sonataloader
{
std::string_view EndFootPopulationLoader::getPopulationType() const noexcept
{
    return EdgeNames::endfoot;
}

void EndFootPopulationLoader::load(EdgeLoadContext &context) const
{
    auto path = EndFeetAreasPath(context).resolve();

    auto &nodeSelection = context.nodeSelection;
    auto nodes = nodeSelection.flatten();
    auto &population = context.edgePopulation;
    auto &edgeSelection = context.edgeSelection;
    auto flatEdges = edgeSelection.flatten();
    auto astrocyteIds = Synapses::getTargetNodes(population, edgeSelection);
    auto endFeetIds = Synapses::getEndFeetIds(population, edgeSelection);

    auto meshes = EndFeetReader::read(path, endFeetIds);

    std::map<uint64_t, std::vector<brayns::TriangleMesh>> endfeetGeometry;
    for (size_t i = 0; i < astrocyteIds.size(); ++i)
    {
        const auto astrocyte = astrocyteIds[i];
        auto &buffer = endfeetGeometry[astrocyte];
        auto &mesh = meshes[i];
        buffer.push_back(std::move(mesh));
    }

    auto buildContext = EndfeetCircuitBuilder::Context{std::move(endfeetGeometry), ColorDataFactory::create(context)};
    EndfeetCircuitBuilder::build(context.model, std::move(buildContext));
}
} // namespace sonataloader

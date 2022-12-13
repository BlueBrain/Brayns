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

#include "CellPlacementLoader.h"

#include "sonataloader/colordata/SonataColorData.h"
#include "sonataloader/data/Cells.h"
#include "sonataloader/data/Config.h"
#include "sonataloader/populations/nodes/common/NeuronMetadataFactory.h"
#include "util/ProgressUpdater.h"

#include <api/ModelType.h>
#include <api/circuit/MorphologyCircuitBuilder.h>

#include <brayns/utils/Log.h>
#include <brayns/utils/Timer.h>

#include <spdlog/fmt/fmt.h>

#include <array>
#include <filesystem>

namespace
{
class MorphologyExtension
{
public:
    static std::string find(const CellPlacementLoaderParameters &parameters, const std::string &morphologyName)
    {
        auto &folder = parameters.morphology_folder;

        if (parameters.extension)
        {
            return _fromExtension(folder, morphologyName, *parameters.extension);
        }

        return _guessExtension(folder, morphologyName);
    }

private:
    static std::string _fromExtension(const std::string &folder, const std::string name, const std::string extension)
    {
        auto testPath = std::filesystem::path(folder) / std::filesystem::path(name + "." + extension);

        if (!std::filesystem::is_regular_file(testPath))
        {
            throw std::invalid_argument(fmt::format("Cannot find .{} morphologies at {}", extension, folder));
        }

        return extension;
    }

    static std::string _guessExtension(const std::string &folder, const std::string name)
    {
        const auto extensionList = std::array<std::string, 3>{"swc", "h5", "swc"};
        for (auto &extension : extensionList)
        {
            auto testPath = std::filesystem::path(folder) / std::filesystem::path(name + "." + extension);
            if (std::filesystem::is_regular_file(testPath))
            {
                return extension;
            }
        }

        throw std::invalid_argument(fmt::format("Could not find morphology files at {}", folder));
    }
};

struct PopulationInfo
{
    std::string path;
    std::string name;
};

class PopulationLoader
{
public:
    static std::shared_ptr<brayns::Model>
        load(const PopulationInfo &info, const CellPlacementLoaderParameters &parameters, ProgressUpdater &updater)
    {
        auto nodes = bbp::sonata::NodePopulation(info.path, "", info.name);
        auto selection = nodes.selectAll();

        auto positions = sonataloader::Cells::getPositions(nodes, selection);
        auto rotations = sonataloader::Cells::getRotations(nodes, selection);
        auto morphologyPaths = _buildMorphologyPaths(parameters, nodes, selection);
        auto geometryType = NeuronGeometryType::Smooth;
        auto morphologyParams = NeuronMorphologyLoaderParameters{1.f, true, false, true, geometryType, 1.f, 0};

        auto context = MorphologyCircuitBuilder::Context{
            selection.flatten(),
            std::move(morphologyPaths),
            std::move(positions),
            std::move(rotations),
            morphologyParams,
            std::make_unique<sonataloader::SonataColorData>(std::move(nodes))};

        auto model = std::make_shared<brayns::Model>(ModelType::neurons);

        updater.beginStage(selection.flatSize());
        MorphologyCircuitBuilder::build(*model, std::move(context), updater);
        sonataloader::NeuronMetadataFactory::create(*model, info.name);
        updater.endStage();

        return model;
    }

private:
    static std::vector<std::string> _buildMorphologyPaths(
        const CellPlacementLoaderParameters &parameters,
        const bbp::sonata::NodePopulation &nodes,
        const bbp::sonata::Selection &selection)
    {
        auto morphologies = sonataloader::Cells::getMorphologies(nodes, selection);
        auto extension = MorphologyExtension::find(parameters, morphologies.front());
        auto builder = sonataloader::MorphologyPath(parameters.morphology_folder, extension);

        auto result = std::vector<std::string>();
        result.reserve(morphologies.size());

        for (auto &morphologyName : morphologies)
        {
            result.push_back(builder.buildPath(morphologyName));
        }

        return result;
    }
};

class StorageLoader
{
public:
    static std::vector<std::shared_ptr<brayns::Model>> fromFile(
        const std::string &path,
        const brayns::LoaderProgress &callback,
        const CellPlacementLoaderParameters &parameters)
    {
        auto nodeStorage = bbp::sonata::NodeStorage(path);
        auto populationNames = nodeStorage.populationNames();

        auto updater = ProgressUpdater(callback, populationNames.size());

        auto result = std::vector<std::shared_ptr<brayns::Model>>();
        result.reserve(populationNames.size());

        for (auto &populationName : populationNames)
        {
            result.push_back(PopulationLoader::load({path, populationName}, parameters, updater));
        }

        return result;
    }
};
}

std::vector<std::string> CellPlacementLoader::getSupportedExtensions() const
{
    return {"circuit.morphologies.h5"};
}

bool CellPlacementLoader::isSupported(const std::string &filename, const std::string &extension) const
{
    (void)extension;
    auto path = std::filesystem::path(filename);
    auto name = path.filename();
    return name.string() == "circuit.morphologies.h5";
}

std::string CellPlacementLoader::getName() const
{
    return "Cell placement loader";
}

std::vector<std::shared_ptr<brayns::Model>> CellPlacementLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &callback,
    const CellPlacementLoaderParameters &params) const
{
    (void)blob;
    (void)callback;
    (void)params;
    throw std::runtime_error("Binary upload is not availble for " + getName());
}

std::vector<std::shared_ptr<brayns::Model>> CellPlacementLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const CellPlacementLoaderParameters &params) const
{
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);
    auto timer = brayns::Timer();

    auto models = StorageLoader::fromFile(path, callback, params);

    brayns::Log::info("[CE] {}: Loaded {} model(s) in {} second(s).", getName(), models.size(), timer.seconds());

    return models;
}

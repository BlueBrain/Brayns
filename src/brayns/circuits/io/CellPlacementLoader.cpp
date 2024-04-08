/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "sonataloader/Selector.h"
#include "sonataloader/colordata/SonataColorData.h"
#include "sonataloader/data/Cells.h"
#include "sonataloader/data/Config.h"
#include "sonataloader/populations/nodes/common/NeuronMetadataFactory.h"
#include "util/ProgressUpdater.h"

#include <brayns/circuits/api/ModelType.h>
#include <brayns/circuits/api/circuit/MorphologyCircuitBuilder.h>
#include <brayns/circuits/api/circuit/SomaCircuitBuilder.h>

#include <brayns/core/network/jsonrpc/JsonRpcException.h>

#include <brayns/core/utils/Log.h>
#include <brayns/core/utils/Timer.h>

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
    static std::string _fromExtension(const std::string &folder, const std::string &name, const std::string &extension)
    {
        auto testPath = std::filesystem::path(folder) / std::filesystem::path(name + "." + extension);

        if (!std::filesystem::is_regular_file(testPath))
        {
            throw std::invalid_argument(fmt::format("Cannot find .{} morphologies at {}", extension, folder));
        }

        return extension;
    }

    static std::string _guessExtension(const std::string &folder, const std::string &name)
    {
        const auto extensionList = std::array<std::string, 3>{"asc", "h5", "swc"};
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

class CellSelector
{
public:
    static bbp::sonata::Selection select(
        const bbp::sonata::NodePopulation &nodes,
        const CellPlacementLoaderParameters &params)
    {
        if (!params.ids.empty())
        {
            return bbp::sonata::Selection::fromValues(params.ids);
        }
        return sonataloader::PercentageFilter::filter(nodes.selectAll(), params.percentage);
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
    static std::shared_ptr<brayns::Model> load(
        const PopulationInfo &info,
        const CellPlacementLoaderParameters &parameters,
        ProgressUpdater &updater)
    {
        auto nodes = bbp::sonata::NodePopulation(info.path, "", info.name);
        auto selection = CellSelector::select(nodes, parameters);

        auto ids = selection.flatten();
        auto positions = sonataloader::Cells::getPositions(nodes, selection);

        auto &morphologyParams = parameters.morphology_parameters;

        auto model = std::make_shared<brayns::Model>(ModelType::neurons);

        auto morphologyEmpty =
            !morphologyParams.load_soma && !morphologyParams.load_dendrites && !morphologyParams.load_axon;

        auto noFolder = parameters.morphology_folder.empty();

        if (noFolder && !morphologyEmpty)
        {
            throw brayns::InvalidParamsException("Morphology folder required to load detailed morphologies");
        }

        if (morphologyEmpty || noFolder)
        {
            auto context = SomaCircuitBuilder::Context{
                std::move(ids),
                std::move(positions),
                std::make_unique<sonataloader::SonataColorData>(std::move(nodes)),
                morphologyParams.radius_multiplier};
            SomaCircuitBuilder::build(*model, std::move(context));
            return model;
        }

        auto rotations = sonataloader::Cells::getRotations(nodes, selection);
        auto morphologyPaths = _buildMorphologyPaths(parameters, nodes, selection);

        auto context = MorphologyCircuitBuilder::Context{
            std::move(ids),
            std::move(morphologyPaths),
            std::move(positions),
            std::move(rotations),
            morphologyParams,
            std::make_unique<sonataloader::SonataColorData>(std::move(nodes))};

        updater.beginStage("Morphology load", selection.flatSize());
        MorphologyCircuitBuilder::build(*model, std::move(context), updater);
        sonataloader::NeuronMetadataFactory::create(*model, info.name);
        updater.endStage();

        updater.end("Generating rendering structures. Might take a while");

        return model;
    }

private:
    static std::vector<std::string> _buildMorphologyPaths(
        const CellPlacementLoaderParameters &parameters,
        const bbp::sonata::NodePopulation &nodes,
        const bbp::sonata::Selection &selection)
    {
        auto morphologies = sonataloader::Cells::getMorphologies(nodes, selection);

        if (morphologies.empty())
        {
            return {};
        }

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
} // namespace

std::string CellPlacementLoader::getName() const
{
    return "Cell placement loader";
}

std::vector<std::string> CellPlacementLoader::getExtensions() const
{
    return {"circuit.morphologies.h5"};
}

std::vector<std::shared_ptr<brayns::Model>> CellPlacementLoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);
    auto &progress = request.progress;
    auto &params = request.params;

    brayns::Log::info("[CE] {}: loading {}.", getName(), path);
    auto timer = brayns::Timer();

    auto models = StorageLoader::fromFile(path, progress, params);

    brayns::Log::info("[CE] {}: Loaded {} model(s) in {} second(s).", getName(), models.size(), timer.seconds());

    return models;
}

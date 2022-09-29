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

#include "CellLoader.h"

#include <brayns/engine/components/Metadata.h>
#include <brayns/utils/string/StringJoiner.h>

#include <api/circuit/MorphologyCircuitBuilder.h>
#include <api/circuit/SomaCircuitBuilder.h>
#include <io/bbploader/colordata/BBPNeuronColorData.h>

#include <brion/blueConfig.h>

#include <filesystem>
#include <future>

namespace
{
class BBPNeuronColorCreator
{
public:
    static std::unique_ptr<bbploader::BBPNeuronColorData> newData(const bbploader::LoadContext &context)
    {
        const auto &config = context.config;
        auto circuitURI = config.getCircuitSource();
        auto circuitPath = circuitURI.getPath();
        if (!std::filesystem::exists(circuitPath))
        {
            circuitURI = config.getCellLibrarySource();
            circuitPath = circuitURI.getPath();
        }
        auto circuitPopulation = config.getCircuitPopulation();
        return std::make_unique<bbploader::BBPNeuronColorData>(std::move(circuitPath), std::move(circuitPopulation));
    }
};

class SomaImporter
{
public:
    static std::vector<CellCompartments> import(const bbploader::LoadContext &context, brayns::Model &model)
    {
        auto colorData = BBPNeuronColorCreator::newData(context);

        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        const auto &params = context.loadParameters;
        const auto &morphParams = params.neuron_morphology_parameters;
        const auto radius = morphParams.radius_multiplier;

        const auto positions = circuit.getPositions(gids);
        const auto ids = std::vector<uint64_t>(gids.begin(), gids.end());

        const SomaCircuitBuilder::Context loadContext(ids, positions, radius);

        return SomaCircuitBuilder::load(loadContext, model, std::move(colorData));
    }
};

class MorphologyPathLoader
{
public:
    static std::vector<std::string> load(const brain::Circuit &circuit, const brain::GIDSet &gids)
    {
        auto morphPaths = circuit.getMorphologyURIs(gids);

        std::vector<std::string> result;
        result.reserve(morphPaths.size());

        for (auto &uri : morphPaths)
        {
            result.push_back(uri.getPath());
        }

        return result;
    }
};

class MorphologyImporter
{
public:
    static auto import(const bbploader::LoadContext &context, brayns::Model &model, ProgressUpdater &updater)
    {
        auto colorData = BBPNeuronColorCreator::newData(context);

        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        const auto &params = context.loadParameters;
        const auto &morphParams = params.neuron_morphology_parameters;

        const auto morphPaths = MorphologyPathLoader::load(circuit, gids);
        const auto positions = circuit.getPositions(gids);
        const auto rotations = circuit.getRotations(gids);
        const auto ids = std::vector<uint64_t>(gids.begin(), gids.end());

        MorphologyCircuitBuilder::Context loadContext(ids, morphPaths, positions, rotations, morphParams);

        return MorphologyCircuitBuilder::load(loadContext, model, updater, std::move(colorData));
    }
};

class MetadataFactory
{
public:
    static void create(const bbploader::LoadContext &context, brayns::Model &dst)
    {
        auto &metadata = dst.getComponents().add<brayns::Metadata>();

        const auto &gids = context.gids;
        const auto &params = context.loadParameters;
        const auto &targets = params.targets;
        if (targets.has_value())
        {
            const auto targetList = brayns::StringJoiner::join(*targets, ",");
            metadata["targets"] = targetList;
        }
        metadata["loaded_neuron_count"] = std::to_string(gids.size());
    }
};
}

namespace bbploader
{
std::vector<CellCompartments>
    CellLoader::load(const LoadContext &context, ProgressUpdater &updater, brayns::Model &model)
{
    const auto &params = context.loadParameters;
    const auto &morphSettings = params.neuron_morphology_parameters;
    const auto loadSoma = morphSettings.load_soma;
    const auto loadAxon = morphSettings.load_axon;
    const auto loadDend = morphSettings.load_dendrites;

    MetadataFactory::create(context, model);

    if (loadSoma && !loadAxon && !loadDend)
    {
        return SomaImporter::import(context, model);
    }

    return MorphologyImporter::import(context, model, updater);
}
} // namespace bbploader

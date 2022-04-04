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

#include <io/bbploader/colordata/BBPNeuronColorData.h>

#include <io/circuit/MorphologyCircuitLoader.h>
#include <io/circuit/SomaCircuitLoader.h>

#include <brion/blueConfig.h>

#include <future>
#include <filesystem>

namespace
{
struct BBPNeuronColorCreator
{
    static std::unique_ptr<bbploader::BBPNeuronColorData> newData(const bbploader::LoadContext& context)
    {
        const auto &config = context.config;
        auto circuitURI = config.getCircuitSource();
        auto circuitPath = circuitURI.getPath();
        if(!std::filesystem::exists(circuitPath))
        {
            circuitURI = config.getCellLibrarySource();
            circuitPath = circuitURI.getPath();
        }
        auto circuitPopulation = config.getCircuitPopulation();
        return std::make_unique<bbploader::BBPNeuronColorData>(std::move(circuitPath), std::move(circuitPopulation));
    }
};

struct SomaImporter
{
    static std::vector<CompartmentStructure> import(const bbploader::LoadContext &context, brayns::Model &model)
    {
        auto colorData = BBPNeuronColorCreator::newData(context);

        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        const auto &params = context.loadParameters;
        const auto &morphParams = params.neuron_morphology_parameters;
        const auto radius = morphParams.radius_multiplier;

        const auto positions = circuit.getPositions(gids);
        const auto ids = std::vector<uint64_t>(gids.begin(), gids.end());

        const SomaCircuitLoader::Context loadContext(ids, positions, radius);

        return SomaCircuitLoader::load(loadContext, model, std::move(colorData));
    }
};

struct MorphologyPathLoader
{
    static std::vector<std::string> load(const brain::Circuit &circuit, const brain::GIDSet &gids)
    {
        const auto morphPaths = circuit.getMorphologyURIs(gids);
        std::vector<std::string> result (morphPaths.size());

#pragma omp parallel for
        for(size_t i = 0; i < morphPaths.size(); ++i)
        {
            const auto &uri = morphPaths[i];
            result[i] = uri.getPath();
        }

        return result;
    }
};

struct MorphologyImporter
{
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

        MorphologyCircuitLoader::Context loadContext(ids, morphPaths, positions, rotations, morphParams);

        return MorphologyCircuitLoader::load(loadContext, model, updater, std::move(colorData));
    }
};
}

namespace bbploader
{
std::vector<CompartmentStructure> CellLoader::load(
        const LoadContext &context, ProgressUpdater &updater, brayns::Model &model)
{
    const auto &params = context.loadParameters;
    const auto &morphSettings = params.neuron_morphology_parameters;
    const auto loadSoma = morphSettings.load_soma;
    const auto loadAxon = morphSettings.load_axon;
    const auto loadDend = morphSettings.load_dendrites;

    if(loadSoma && !loadAxon && !loadDend)
    {
        return SomaImporter::import(context, model);
    }
    else
    {
        return MorphologyImporter::import(context, model, updater);
    }
}
} // namespace bbploader

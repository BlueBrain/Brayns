/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "BBPLoader.h"

#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>
#include <brayns/engine/Scene.h>
#include <brayns/utils/Filesystem.h>

#include <plugin/api/CircuitColorManager.h>
#include <plugin/api/MaterialUtils.h>
#include <plugin/io/bbploader/CellLoader.h>
#include <plugin/io/bbploader/ParameterCheck.h>
#include <plugin/io/bbploader/SynapseLoader.h>
#include <plugin/io/bbploader/colorhandlers/BBPNeuronColorHandler.h>
#include <plugin/io/bbploader/colorhandlers/BBPSynapseColorHandler.h>
#include <plugin/io/bbploader/simulation/CompartmentSimulation.h>
#include <plugin/io/bbploader/simulation/SpikeSimulation.h>
#include <plugin/io/util/TransferFunctionUtils.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <fstream>
#include <iostream>
#include <unistd.h>

using namespace bbploader;

namespace
{
std::unique_ptr<Simulation> intantiateSimulation(
    const brion::BlueConfig& config, const BBPLoaderParameters& input,
    const brain::GIDSet& inputGids) noexcept
{
    switch (input.report_type)
    {
    case SimulationType::COMPARTMENT:
        return std::make_unique<CompartmentSimulation>(
            config.getReportSource(input.report_name).getPath(), inputGids);
    case SimulationType::SPIKES:
        return std::make_unique<SpikeSimulation>(
            config.getSpikeSource().getPath(), inputGids,
            input.spike_transition_time);
    default:
        return {nullptr};
    }
}

brain::GIDSet computeInitialGIDs(const brion::BlueConfig& config,
                                 const brain::Circuit& circuit,
                                 const BBPLoaderParameters& input)
{
    if (!input.gids.empty())
        return brain::GIDSet(input.gids.begin(), input.gids.end());

    brain::GIDSet result;
    std::vector<std::string> targets;
    if (!input.targets.empty())
        targets = input.targets;
    else
        targets = {config.getCircuitTarget()};

    brain::GIDSet allGids;
    for (const auto& target : targets)
    {
        const auto tempGids = circuit.getGIDs(target);
        allGids.insert(tempGids.begin(), tempGids.end());
    }

    if (input.percentage >= 1.f)
        return allGids;

    const auto expectedSize =
        static_cast<size_t>(allGids.size() * input.percentage);
    const auto skipFactor = static_cast<size_t>(
        static_cast<float>(allGids.size()) / static_cast<float>(expectedSize));
    brain::GIDSet finalList;
    auto it = finalList.begin();
    auto allIt = allGids.begin();
    while (allIt != allGids.end())
    {
        finalList.insert(it, *allIt);
        ++it;
        size_t counter{0};
        while (counter++ < skipFactor && allIt != allGids.end())
            ++allIt;
    }

    return finalList;
}

brayns::ModelDescriptorPtr loadSynapse(
    const std::string& path, const brain::Circuit& circuit,
    const brain::GIDSet& gids, const bool afferent,
    const std::vector<MorphologyInstance::Ptr>& cells, brayns::ModelPtr&& model)
{
    auto synapses = SynapseLoader::load(circuit, gids, afferent);
    if (synapses.empty())
    {
        brayns::Log::warn("[CE] synapses empty.");
        return {nullptr};
    }

    std::vector<ElementMaterialMap::Ptr> synapseMatMap(gids.size());

#pragma omp parallel for
    for (size_t i = 0; i < cells.size(); ++i)
        synapses[i]->mapToCell(*cells[i]);

    for (size_t i = 0; i < synapses.size(); ++i)
    {
        synapseMatMap[i] = synapses[i]->addToModel(*model);
        synapses[i].reset(nullptr);
    }

    if (model->empty())
        return {nullptr};

    model->updateBounds();

    brayns::Transformation transformation;
    transformation.setRotationCenter(model->getBounds().getCenter());
    auto modelDesc =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "Synapses",
                                                  path,
                                                  brayns::ModelMetadata());
    modelDesc->setTransformation(transformation);
    modelDesc->setName(afferent ? "Afferent synapses" : "Efferent synapses");

    auto synapseColor = std::make_unique<BBPSynapseColorHandler>();
    const auto ids = std::vector<uint64_t>(gids.begin(), gids.end());
    CircuitColorManager::registerHandler(modelDesc, std::move(synapseColor),
                                         ids, std::move(synapseMatMap));

    return modelDesc;
}

std::string getCircuitFilePath(const brion::BlueConfig& config)
{
    const auto csrc = config.getCircuitSource().getPath();
    if (fs::exists(csrc))
        return csrc;
    const auto ssrc = config.getCellLibrarySource().getPath();
    if (fs::exists(ssrc))
        return ssrc;

    return "";
}
} // namespace

std::vector<std::string> BBPLoader::getSupportedExtensions() const
{
    static const strings LOADER_EXTENSIONS = {"BlueConfig", "BlueConfig3",
                                              "CircuitConfig",
                                              "CircuitConfig_nrn"};
    return LOADER_EXTENSIONS;
}

bool BBPLoader::isSupported(const std::string& filename,
                            const std::string& extension) const
{
    const auto containsKeyword = [](const std::string& matcher) {
        const auto lcm = brayns::string_utils::toLowercase(matcher);
        if (lcm.find("blueconfig") != std::string::npos ||
            lcm.find("circuitconfig") != std::string::npos)
            return true;

        return false;
    };

    return containsKeyword(fs::path(filename).filename()) ||
           containsKeyword(extension);
}

std::string BBPLoader::getName() const
{
    return "BBP loader";
}

std::vector<brayns::ModelDescriptorPtr> BBPLoader::importFromBlob(
    brayns::Blob&&, const brayns::LoaderProgress&, const BBPLoaderParameters&,
    brayns::Scene&) const
{
    throw std::runtime_error("BBP loader: import from blob not supported");
}

std::vector<brayns::ModelDescriptorPtr> BBPLoader::importFromFile(
    const std::string& path, const brayns::LoaderProgress& callback,
    const BBPLoaderParameters& params, brayns::Scene& scene) const
{
    brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    const brion::BlueConfig config(path);
    const auto result =
        importFromBlueConfig(path, callback, params, config, scene);

    brayns::Log::info("[CE] {}: done in {} second(s).", getName(),
                      timer.elapsed());
    return result;
}

std::vector<brayns::ModelDescriptorPtr> BBPLoader::importFromBlueConfig(
    const std::string& path, const brayns::LoaderProgress& callback,
    const BBPLoaderParameters& params, const brion::BlueConfig& config,
    brayns::Scene& scene) const
{
    std::vector<brayns::ModelDescriptorPtr> result;

    // INITIALIZE DATA ACCESSORS
    // -------------------------------------------------------------------------------------------
    const brain::Circuit circuit(config);
    ParameterCheck::checkInput(config, params);

    brayns::ModelPtr cellModel = scene.createModel();

    // Configure progress reporter
    const float chunk = 0.2f;
    float total = 0.f;

    callback.updateProgress("Loading cells", total);
    total += chunk;
    auto gids = computeInitialGIDs(config, circuit, params);

    // Load simulation, intersect initial gids with simulation gids
    const auto simulation = intantiateSimulation(config, params, gids);
    if (simulation)
        gids = simulation->getReportGids();

    if (gids.empty())
        throw std::runtime_error(
            "BBPLoader: No GIDs selected. Empty circuits not supported");

    const std::vector<uint64_t> gidList(gids.begin(), gids.end());

    auto cells = CellLoader::load(params, gids, circuit);

    callback.updateProgress("Loading simulation", total);
    total += chunk;
    if (simulation)
    {
        const auto mapping = simulation->getMapping(gids);
#pragma omp parallel for
        for (size_t i = 0; i < mapping.size(); ++i)
        {
            const auto& cm = mapping[i];
            cells[i]->mapSimulation(cm.globalOffset, cm.offsets,
                                    cm.compartments);
        }
        cellModel->setSimulationHandler(simulation->createHandler());
        TransferFunctionUtils::set(scene.getTransferFunction());
    }

    if (params.load_afferent_synapses)
    {
        brayns::Log::info("[CE] {}: loading afferent synapses.", getName());
        callback.updateProgress("Loading afferent synapses", total);
        total += chunk;
        auto model =
            loadSynapse(path, circuit, gids, true, cells, scene.createModel());
        if (model)
        {
            // Enable simulation for synapses as well
            model->getModel().setSimulationHandler(
                cellModel->getSimulationHandler());
            result.push_back(model);
        }
    }

    if (params.load_efferent_synapses)
    {
        brayns::Log::info("[CE] {}: loading efferent synapses.", getName());
        callback.updateProgress("Loading efferent synapses", total);
        auto model =
            loadSynapse(path, circuit, gids, false, cells, scene.createModel());
        if (model)
        {
            model->getModel().setSimulationHandler(
                cellModel->getSimulationHandler());
            result.push_back(model);
        }
    }

    callback.updateProgress("Generating geometry", total);
    std::vector<ElementMaterialMap::Ptr> cellMatMap(gids.size());
    for (size_t i = 0; i < cells.size(); ++i)
    {
        cellMatMap[i] = cells[i]->addToModel(*cellModel);
        cells[i].reset(nullptr);
    }

    cellModel->updateBounds();

    brayns::Transformation transformation;
    transformation.setRotationCenter(cellModel->getBounds().getCenter());
    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(cellModel),
                                                  "Circuit", path,
                                                  brayns::ModelMetadata());
    modelDescriptor->setTransformation(transformation);
    result.push_back(modelDescriptor);

    // Enable simulation coloring for all models if there is a simulation loaded
    if (simulation)
        for (auto& model : result)
            CircuitExplorerMaterial::setSimulationColorEnabled(
                model->getModel(), true);

    auto cellColor =
        std::make_unique<BBPNeuronColorHandler>(getCircuitFilePath(config),
                                                config.getCircuitPopulation());
    CircuitColorManager::registerHandler(modelDescriptor, std::move(cellColor),
                                         gidList, std::move(cellMatMap));

    brayns::Log::info("[CE] {}: loaded {} model(s).", getName(), result.size());

    return result;
}

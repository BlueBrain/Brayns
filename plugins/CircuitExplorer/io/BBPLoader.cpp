/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <io/bbploader/CellLoader.h>
#include <io/bbploader/GIDLoader.h>
#include <io/bbploader/LoadContext.h>
#include <io/bbploader/ParameterCheck.h>
#include <io/bbploader/SimulationLoader.h>
#include <io/bbploader/SynapseLoader.h>
#include <io/util/ProgressUpdater.h>

#include <filesystem>

namespace
{
struct SynapseImporter
{
    static std::unique_ptr<brayns::Model> import(const bbploader::LoadContext &context, bool post)
    {
        auto model = std::make_unique<brayns::Model>();
        bbploader::SynapseLoader::load(context, post, *model);
        return model;
    }
};
}

std::vector<std::string> BBPLoader::getSupportedExtensions() const
{
    return {"BlueConfig", "CircuitConfig"};
}

bool BBPLoader::isSupported(const std::string &filename, const std::string &extension) const
{
    const auto containsKeyword = [](const std::string &matcher)
    {
        const auto lcm = brayns::string_utils::toLowercase(matcher);
        if (lcm.find("blueconfig") != std::string::npos || lcm.find("circuitconfig") != std::string::npos)
        {
            return true;
        }

        return false;
    };

    return containsKeyword(std::filesystem::path(filename).filename()) || containsKeyword(extension);
}

std::string BBPLoader::getName() const
{
    return "BBP loader";
}

std::vector<std::unique_ptr<brayns::Model>> BBPLoader::importFromBlob(
    brayns::Blob &&blob,
    const brayns::LoaderProgress &cb,
    const BBPLoaderParameters &params) const
{
    (void) blob;
    (void) cb;
    (void) params;
    throw std::runtime_error("BBP loader: import from blob not supported");
}

std::vector<std::unique_ptr<brayns::Model>> BBPLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const BBPLoaderParameters &params) const
{
    brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    const brion::BlueConfig config(path);
    auto result = importFromBlueConfig(callback, params, config);

    brayns::Log::info("[CE] {}: done in {} second(s).", getName(), timer.seconds());
    return result;
}

std::vector<std::unique_ptr<brayns::Model>> BBPLoader::importFromBlueConfig(
    const brayns::LoaderProgress &callback,
    const BBPLoaderParameters &params,
    const brion::BlueConfig &config) const
{
    bbploader::ParameterCheck::checkInput(config, params);

    const brain::Circuit circuit(config);
    const auto gids = bbploader::GIDLoader::compute(config, circuit, params);
    const bbploader::LoadContext context (circuit, gids, config, params);

    std::vector<std::unique_ptr<brayns::Model>> result;

    // Configure progress reporter
    const float chunk = 0.2f;
    float total = 0.f;

    // Compute GIDs
    callback.updateProgress("Computing GIDs", total);

    const std::vector<uint64_t> gidList(gids.begin(), gids.end());

    // Load neurons
    total += chunk;
    callback.updateProgress("Loading neurons", total);
    auto cellModel = std::make_unique<brayns::Model>();
    ProgressUpdater updater (callback, total, total + chunk, gidList.size());
    auto compartments = bbploader::CellLoader::load(context, updater, *cellModel);

    // Load simulation
    total += chunk;
    if(params.report_type != bbploader::SimulationType::NONE)
    {
        callback.updateProgress("Loading simulation", total);
        bbploader::SimulationLoader::load(context, compartments, *cellModel);
    }
    result.push_back(std::move(cellModel));

    // Load afferent synapses
    total += chunk;
    const auto loadAfferent = params.load_afferent_synapses;
    if (loadAfferent)
    {
        brayns::Log::info("[CE] {}: loading afferent synapses.", getName());
        callback.updateProgress("Loading afferent synapses", total);

        auto model = SynapseImporter::import(context, true);
        result.push_back(std::move(model));
    }

    // Load efferent synapses
    total += chunk;
    const auto loadEfferent = params.load_efferent_synapses;
    if (loadEfferent)
    {
        brayns::Log::info("[CE] {}: loading efferent synapses.", getName());
        callback.updateProgress("Loading efferent synapses", total);

        auto model = SynapseImporter::import(context, false);
        result.push_back(std::move(model));
    }

    total += chunk;
    callback.updateProgress("Done", total);
    brayns::Log::info("[CE] {}: loaded {} model(s).", getName(), result.size());

    return result;
}

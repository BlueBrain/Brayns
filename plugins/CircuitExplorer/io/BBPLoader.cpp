/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/utils/Log.h>
#include <brayns/utils/Timer.h>
#include <brayns/utils/string/StringCase.h>

#include <api/ModelType.h>
#include <io/bbploader/CellLoader.h>
#include <io/bbploader/GIDLoader.h>
#include <io/bbploader/LoadContext.h>
#include <io/bbploader/ParameterCheck.h>
#include <io/bbploader/ReportLoader.h>
#include <io/bbploader/SynapseLoader.h>
#include <io/util/ProgressUpdater.h>

#include <filesystem>

namespace
{
struct SynapseImporter
{
    static void import(
        const bbploader::LoadContext &context,
        std::vector<std::shared_ptr<brayns::Model>> &modelList,
        ProgressUpdater &updater)
    {
        const auto &params = context.loadParameters;
        const auto afferent = params.load_afferent_synapses;
        const auto efferent = params.load_efferent_synapses;

        if (afferent)
        {
            updater.update();
            auto model = bbploader::SynapseLoader::load(context, true);
            modelList.push_back(std::move(model));
        }
        if (efferent)
        {
            updater.update();
            auto model = bbploader::SynapseLoader::load(context, false);
            modelList.push_back(std::move(model));
        }
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
        const auto lcm = brayns::StringCase::toLower(matcher);
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

std::vector<std::shared_ptr<brayns::Model>> BBPLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &callback,
    const BBPLoaderParameters &params) const
{
    (void)blob;
    (void)callback;
    (void)params;
    throw std::runtime_error("BBP loader: import from blob not supported");
}

std::vector<std::shared_ptr<brayns::Model>> BBPLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const BBPLoaderParameters &params) const
{
    brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    const brion::BlueConfig config(path);
    bbploader::ParameterCheck::checkInput(config, params);

    ProgressUpdater updater(callback, 3, 0.5f);

    const brain::Circuit circuit(config);
    const auto gids = bbploader::GIDLoader::compute(config, circuit, params);
    const bbploader::LoadContext context{circuit, gids, config, params};

    std::vector<std::shared_ptr<brayns::Model>> result;

    auto model = std::make_shared<brayns::Model>(ModelType::neurons);

    // Load neurons
    updater.beginStage("Neuron load", gids.size());
    auto compartments = bbploader::CellLoader::load(context, updater, *model);
    updater.endStage();

    // Load simulation
    updater.beginStage("Report load");
    bbploader::ReportLoader::load(context, compartments, updater, *model);
    updater.endStage();

    result.push_back(std::move(model));

    // Load synapses
    updater.beginStage("Synapse load", 2);
    SynapseImporter::import(context, result, updater);
    updater.endStage();

    updater.end("Generating rendering structures. Might take a while");

    brayns::Log::info("[CE] {}: Loaded {} model(s) in {} second(s).", getName(), result.size(), timer.seconds());

    return result;
}

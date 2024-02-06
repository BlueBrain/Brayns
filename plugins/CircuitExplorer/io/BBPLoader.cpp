/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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
class SynapseImporter
{
public:
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
} // namespace

std::string BBPLoader::getName() const
{
    return "BBP loader";
}

std::vector<std::string> BBPLoader::getExtensions() const
{
    return {"BlueConfig", "CircuitConfig"};
}

std::vector<std::shared_ptr<brayns::Model>> BBPLoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);
    auto &callback = request.progress;
    auto &params = request.params;

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

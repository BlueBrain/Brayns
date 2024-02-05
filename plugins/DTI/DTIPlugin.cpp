/* Copyright 2018-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns
 * <https://github.com/BlueBrain/Brayns-UC-DTI>
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

#include "DTIPlugin.h"

#include <io/DTILoader.h>
#include <io/DTISimulationHandler.h>
#include <io/DTITypes.h>

#include <brayns/common/Log.h>
#include <brayns/common/Progress.h>
#include <brayns/common/geometry/Streamline.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include "DtiEntrypoints.h"

namespace
{
const size_t DEFAULT_MATERIAL_ID = 0;
const std::string MATERIAL_PROPERTY_SHADING_MODE = "shading_mode";
} // namespace

namespace dti
{
DTIPlugin::DTIPlugin()
    : brayns::ExtensionPlugin("DTI")
{
}

void DTIPlugin::init()
{
    auto &registry = _api->getLoaderRegistry();

    registry.registerLoader(std::make_unique<DTILoader>());

    DtiEntrypoints::load(*this);
}

void DTIPlugin::preRender()
{
    if (_simulationDirty)
    {
        _updateSpikeSimulation();
        _simulationDirty = false;
    }

    auto &ap = _api->getParametersManager().getAnimationParameters();
    if (ap.getFrame() != _currentFrame)
    {
        _currentFrame = ap.getFrame();
        _updateSimulationFrame();
    }
}

void DTIPlugin::updateSpikeSimulation(
    const SetSpikeSimulationMessage &spikeSimulation)

{
    auto modelDescriptor = _api->getScene().getModel(spikeSimulation.model_id);
    if (!modelDescriptor)
    {
        brayns::Log::error("[DTI] {} is an invalid model ID.",
                           spikeSimulation.model_id);
        return;
    }
    _spikeSimulation = spikeSimulation;
    _simulationDirty = true;
}

void DTIPlugin::updateSpikeSimulationFromFile(
    const SetSpikeSimulationFromFileMessage &src)
{
    auto modelDescriptor = _api->getScene().getModel(src.model_id);
    if (!modelDescriptor)
    {
        brayns::Log::error("[DTI] {} is an invalid model ID.", src.model_id);
        return;
    }

    std::unique_ptr<brion::BlueConfig> config{nullptr};
    try
    {
        config = std::make_unique<brion::BlueConfig>(src.path);
    }
    catch (...)
    {
        brayns::Log::error("[DTI] Cannot read BlueConfig file: '{}'.",
                           src.path);
    }
    std::unique_ptr<brain::SpikeReportReader> spikeReport{nullptr};
    try
    {
        spikeReport = std::make_unique<brain::SpikeReportReader>(
            config->getSpikeSource());
    }
    catch (const std::exception &e)
    {
        brayns::Log::error("[DTI] Could not read Spike report: {}.", e.what());
    }

    _spikeSimulation.dt = src.dt;
    _spikeSimulation.end_time = spikeReport->getEndTime();
    _spikeSimulation.model_id = src.model_id;
    _spikeSimulation.time_scale = src.time_scale;
    _spikeSimulation.decay_speed = src.decay_speed;
    _spikeSimulation.rest_intensity = src.rest_intensity;
    _spikeSimulation.spike_intensity = src.spike_intensity;

    auto spikes = spikeReport->getSpikes(0.f, spikeReport->getEndTime());
    _spikeSimulation.gids.resize(spikes.size());
    _spikeSimulation.timestamps.resize(spikes.size());
    for (size_t i = 0; i < spikes.size(); ++i)
        _spikeSimulation.timestamps[i] = spikes[i].first;

    for (size_t i = 0; i < spikes.size(); ++i)
        _spikeSimulation.gids[i] = spikes[i].second;

    _simulationDirty = true;
}

void DTIPlugin::_updateSpikeSimulation()
{
    auto modelDescriptor = _api->getScene().getModel(_spikeSimulation.model_id);
    auto &model = modelDescriptor->getModel();
    auto simulationHandler = model.getSimulationHandler();
    if (!simulationHandler)
    {
        Indices indices;
        uint64_t count = 0;
        for (const auto &streamlines : model.getStreamlines())
        {
            count += streamlines.second.vertex.size();
            indices.push_back(count);
        }

        brayns::Log::info(
            "[DTI] Creating spike simulation handler for model {}.",
            _spikeSimulation.model_id);
        simulationHandler =
            std::make_shared<DTISimulationHandler>(indices, _spikeSimulation);
        model.setSimulationHandler(simulationHandler);
        _registeredModelsForSpikeSimulation.push_back(
            _spikeSimulation.model_id);
    }

    const auto nbSpikes = _spikeSimulation.gids.size();
    brayns::Log::info("[DTI] Loading {} spikes from JSON to model {}.",
                      nbSpikes, _spikeSimulation.model_id);

    auto *spikesHandler =
        dynamic_cast<DTISimulationHandler *>(simulationHandler.get());
    spikesHandler->setTimeScale(_spikeSimulation.time_scale);
    spikesHandler->setDecaySpeed(_spikeSimulation.decay_speed);
    spikesHandler->setRestIntensity(_spikeSimulation.rest_intensity);
    spikesHandler->setSpikeIntensity(_spikeSimulation.spike_intensity);

    auto &spikes = spikesHandler->getSpikes();
    for (size_t i = 0; i < _spikeSimulation.gids.size(); ++i)
        spikes[_spikeSimulation.gids[i]] = _spikeSimulation.timestamps[i];
}

void DTIPlugin::_updateSimulationFrame()
{
    for (const auto &modelId : _registeredModelsForSpikeSimulation)
    {
        auto modelDescriptor = _api->getScene().getModel(modelId);
        if (!modelDescriptor)
            continue; // Model has probably been deleted

        auto &model = modelDescriptor->getModel();
        auto simulationHandler = model.getSimulationHandler();

        float *data = (float *)simulationHandler->getFrameData(_currentFrame);
        auto &streamlines = model.getStreamlines();
        uint64_t index = 0;
        for (auto &streamline : streamlines)
        {
            auto &color = streamline.second.vertexColor;
            for (uint64_t i = 1; i < streamline.second.vertexColor.size(); ++i)
            {
                const auto simulationValue = data[index + i];

                const auto &v1 = streamline.second.vertex[i - 1];
                const auto &v2 = streamline.second.vertex[i];
                const auto v = normalize(v2 - v1);
                const brayns::Vector3f normal = {0.5f + v.x * 0.5f,
                                                 0.5f + v.y * 0.5f,
                                                 0.5f + v.z * 0.5f};
                color[i].x = normal.x * simulationValue;
                color[i].y = normal.y * simulationValue;
                color[i].z = normal.z * simulationValue;
                if (i == 1)
                {
                    color[0].x = normal.x * simulationValue;
                    color[0].y = normal.y * simulationValue;
                    color[0].z = normal.z * simulationValue;
                }
            }
            index += streamline.second.vertex.size();
        }
    }
}
} // namespace dti

extern "C" brayns::ExtensionPlugin *brayns_plugin_create(int /*argc*/,
                                                         char ** /*argv*/)
{
    brayns::Log::info("[DTI] Loading DTI plugin.");
    return new dti::DTIPlugin();
}

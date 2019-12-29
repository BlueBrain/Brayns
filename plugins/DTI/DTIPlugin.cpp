/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
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
#include <log.h>

#include <brayns/common/ActionInterface.h>
#include <brayns/common/Progress.h>
#include <brayns/common/geometry/Streamline.h>
#include <brayns/engineapi/Camera.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/engineapi/Material.h>
#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/Plugin.h>

namespace
{
const size_t DEFAULT_MATERIAL_ID = 0;
const std::string MATERIAL_PROPERTY_SHADING_MODE = "shading_mode";
} // namespace

namespace dti
{
DTIPlugin::DTIPlugin()
    : ExtensionPlugin()
{
}

void DTIPlugin::init()
{
    auto &scene = _api->getScene();
    auto &registry = scene.getLoaderRegistry();

    registry.registerLoader(
        std::make_unique<DTILoader>(scene, DTILoader::getCLIProperties()));

    PLUGIN_INFO << "Registering 'add-streamlines' endpoint" << std::endl;
    _api->getActionInterface()->registerNotification<StreamlinesDescriptor>(
        "add-streamlines",
        [&](const StreamlinesDescriptor &s) { _updateStreamlines(s); });

    PLUGIN_INFO << "Registering 'set-spike-simulation' endpoint" << std::endl;
    _api->getActionInterface()->registerNotification<SpikeSimulationDescriptor>(
        "set-spike-simulation",
        [&](const SpikeSimulationDescriptor &s) { _updateSpikeSimulation(s); });
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

void DTIPlugin::_updateStreamlines(const StreamlinesDescriptor &streamlines)
{
    const std::string name = streamlines.name;
    PLUGIN_INFO << "Loading streamlines <" << name << "> from Json"
                << std::endl;

    size_t nbStreamlines = 0;
    auto model = _api->getScene().createModel();

    const auto nbIndices = streamlines.indices.size();
    const auto nbVertices = streamlines.vertices.size() / 3;

    uint64_t startIndex = 0;
    for (size_t index = 0; index < nbIndices; ++index)
    {
        // Create material
        const auto materialId =
            streamlines.gids.empty() ? 0 : streamlines.gids[index];
        brayns::PropertyMap props;
        props.setProperty({MATERIAL_PROPERTY_SHADING_MODE, 0});
        auto material =
            model->createMaterial(materialId, std::to_string(materialId),
                                  props);
        material->setOpacity(streamlines.opacity);
        material->setSpecularColor({0, 0, 0});

        // Create streamline geometry
        const auto endIndex = streamlines.indices[index];

        if (endIndex - startIndex < 2)
            // Ignore streamlines with less than 2 points
            continue;

        brayns::Vector3fs points;
        std::vector<float> radii;
        brayns::Vector3f normal;
        for (uint64_t p = startIndex; p < endIndex; ++p)
        {
            const auto i = p * 3;
            const brayns::Vector3f point = {streamlines.vertices[i],
                                            streamlines.vertices[i + 1],
                                            streamlines.vertices[i + 2]};
            points.push_back(point);
            radii.push_back(streamlines.radius);
        }
        const auto colors =
            DTILoader::getColorsFromPoints(points, streamlines.opacity,
                                           streamlines.colorScheme);

        brayns::Streamline streamline(points, colors, radii);
        model->addStreamline(materialId, streamline);
        startIndex = endIndex;

        ++nbStreamlines;
    }

    if (nbStreamlines == 0)
    {
        PLUGIN_INFO << "No streamlines" << std::endl;
        return;
    }

    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), name);
    _api->getScene().addModel(modelDescriptor);

    PLUGIN_INFO << nbStreamlines << " streamlines loaded" << std::endl;
}

void DTIPlugin::_updateSpikeSimulation()
{
    auto modelDescriptor = _api->getScene().getModel(_spikeSimulation.modelId);
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

        PLUGIN_INFO << "Creating spike simulation handler for model "
                    << _spikeSimulation.modelId << std::endl;
        simulationHandler =
            std::make_shared<DTISimulationHandler>(indices, _spikeSimulation);
        model.setSimulationHandler(simulationHandler);
        _registeredModelsForSpikeSimulation.push_back(_spikeSimulation.modelId);
    }

    const auto nbSpikes = _spikeSimulation.gids.size();
    PLUGIN_INFO << "Loading " << nbSpikes << " spikes from JSon to model "
                << _spikeSimulation.modelId << std::endl;

    auto *spikesHandler =
        dynamic_cast<DTISimulationHandler *>(simulationHandler.get());
    spikesHandler->setTimeScale(_spikeSimulation.timeScale);
    spikesHandler->setDecaySpeed(_spikeSimulation.decaySpeed);
    spikesHandler->setRestIntensity(_spikeSimulation.restIntensity);
    spikesHandler->setSpikeIntensity(_spikeSimulation.spikeIntensity);

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

void DTIPlugin::_updateSpikeSimulation(
    const SpikeSimulationDescriptor &spikeSimulation)

{
    auto modelDescriptor = _api->getScene().getModel(spikeSimulation.modelId);
    if (!modelDescriptor)
    {
        PLUGIN_ERROR << spikeSimulation.modelId << " is an invalid model ID"
                     << std::endl;
        return;
    }
    _spikeSimulation = spikeSimulation;
    _simulationDirty = true;
}

} // namespace dti

extern "C" brayns::ExtensionPlugin *brayns_plugin_create(int /*argc*/,
                                                         char ** /*argv*/)
{
    PLUGIN_INFO << "Initializing DTI plugin" << std::endl;
    return new dti::DTIPlugin();
}

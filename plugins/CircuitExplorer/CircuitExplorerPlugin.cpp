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

#include "CircuitExplorerPlugin.h"

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/EntrypointBuilder.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/pluginapi/PluginAPI.h>

#include <api/CircuitColorManager.h>
#include <api/VasculatureRadiiSimulation.h>

#include <io/BBPLoader.h>
#include <io/NeuronMorphologyLoader.h>
#include <io/SonataLoader.h>
#include <io/SonataNGVLoader.h>

#include <network/entrypoints/ColorCircuitEntrypoint.h>
#include <network/entrypoints/MakeMovieEntrypoint.h>
#include <network/entrypoints/MirrorModelEntrypoint.h>
#include <network/entrypoints/SetCircuitThicknessEntrypoint.h>
#include <network/entrypoints/SimulationColorEntrypoint.h>
#include <network/entrypoints/TraceAnterogradeEntrypoint.h>

void CircuitExplorerPlugin::init()
{
    // LOADERS ADDED BY THIS PLUGIN
    auto &registry = _api->getLoaderRegistry();
    registry.registerLoader(std::make_unique<BBPLoader>(_colorManager));
    registry.registerLoader(std::make_unique<NeuronMorphologyLoader>());
    registry.registerLoader(std::make_unique<SonataLoader>(_colorManager));
    registry.registerLoader(std::make_unique<SonataNGVLoader>(_colorManager));
}

void CircuitExplorerPlugin::registerEntrypoints(brayns::INetworkInterface &interface)
{
    auto &engine = _api->getEngine();
    auto &scene = engine.getScene();

    auto builder = brayns::EntrypointBuilder("Circuit Explorer", interface);

    builder.add<AvailableColorMethodsEntrypoint>(scene, _colorManager);
    builder.add<AvailableColorMethodVariablesEntrypoint>(scene, _colorManager);
    builder.add<ColorCircuitByIdEntrypoint>(scene, _colorManager);
    builder.add<ColorCircuitByMethodEntrypoint>(scene, _colorManager);
    builder.add<ColorCircuitBySingleColorEntrypoint>(scene, _colorManager);
    builder.add<MakeMovieEntrypoint>();
    builder.add<MirrorModelEntrypoint>(scene);
    builder.add<SetCircuitThicknessEntrypoint>(scene);
    builder.add<SimulationColorEntrypoint>(scene);
    builder.add<TraceAnterogradeEntrypoint>(scene, _colorManager);
}

void CircuitExplorerPlugin::preRender()
{
    auto &scene = _api->getScene();
    const auto frame = _api->getParametersManager().getAnimationParameters().getFrame();
    VasculatureRadiiSimulation::update(frame, scene);
}

void CircuitExplorerPlugin::postRender()
{
}

extern "C" brayns::ExtensionPlugin *brayns_plugin_create(int, char **)
{
    brayns::Log::info("[CE] Initializing circuit explorer plugin.");
    return new CircuitExplorerPlugin();
}

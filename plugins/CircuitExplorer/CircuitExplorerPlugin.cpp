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

#include <io/BBPLoader.h>
#include <io/NeuronMorphologyLoader.h>
#include <io/SonataLoader.h>
#include <io/SonataNGVLoader.h>
#include <network/entrypoints/ColorCircuitEntrypoint.h>
#include <network/entrypoints/GetCircuitIdsEntrypoint.h>
#include <network/entrypoints/SetCircuitThicknessEntrypoint.h>
#include <network/entrypoints/TraceAnterogradeEntrypoint.h>

CircuitExplorerPlugin::CircuitExplorerPlugin(brayns::PluginAPI &api)
    : _api(api)
{
}

void CircuitExplorerPlugin::onCreate()
{
    auto &registry = _api.getLoaderRegistry();
    registry.registerLoader(std::make_unique<BBPLoader>());
    registry.registerLoader(std::make_unique<NeuronMorphologyLoader>());
    registry.registerLoader(std::make_unique<SonataLoader>());
    registry.registerLoader(std::make_unique<SonataNGVLoader>());
}

void CircuitExplorerPlugin::registerEntrypoints(brayns::INetworkInterface &interface)
{
    auto &engine = _api.getEngine();
    auto &scene = engine.getScene();
    auto &models = scene.getModels();

    auto builder = brayns::EntrypointBuilder("Circuit Explorer", interface);

    builder.add<AvailableColorMethodsEntrypoint>(models);
    builder.add<AvailableColorMethodVariablesEntrypoint>(models);
    builder.add<ColorCircuitByIdEntrypoint>(models);
    builder.add<ColorCircuitByMethodEntrypoint>(models);
    builder.add<ColorCircuitBySingleColorEntrypoint>(models);
    builder.add<GetCircuitIdsEntrypoint>(models);
    builder.add<SetCircuitThicknessEntrypoint>(models);
    builder.add<TraceAnterogradeEntrypoint>(models);
}

extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(brayns::PluginAPI &api)
{
    brayns::Log::info("[CE] Loading Circuit Explorer plugin.");
    return std::make_unique<CircuitExplorerPlugin>(api);
}

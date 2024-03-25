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

#include "CircuitExplorerPlugin.h"

#include <brayns/network/entrypoint/EntrypointBuilder.h>
#include <brayns/utils/Log.h>

#include <io/CellPlacementLoader.h>
#include <io/NeuronMorphologyLoader.h>
#include <io/SonataLoader.h>
#include <network/entrypoints/GetCircuitIdsEntrypoint.h>
#include <network/entrypoints/SetCircuitThicknessEntrypoint.h>

CircuitExplorerPlugin::CircuitExplorerPlugin(brayns::PluginAPI &api)
{
    auto name = "Circuit Explorer";

    auto &registry = api.getLoaderRegistry();
    auto loaders = brayns::LoaderRegistryBuilder(name, registry);

    loaders.add<CellPlacementLoader>();
    loaders.add<NeuronMorphologyLoader>();
    loaders.add<SonataLoader>();

    auto interface = api.getNetworkInterface();
    if (!interface)
    {
        return;
    }
    auto &engine = api.getEngine();
    auto &scene = engine.getScene();
    auto &models = scene.getModels();
    auto entrypoints = brayns::EntrypointBuilder(name, *interface);

    entrypoints.add<GetCircuitIdsEntrypoint>(models);
    entrypoints.add<SetCircuitThicknessEntrypoint>(models);
}

extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(brayns::PluginAPI &api)
{
    brayns::Log::info("[CE] Loading Circuit Explorer plugin.");
    return std::make_unique<CircuitExplorerPlugin>(api);
}

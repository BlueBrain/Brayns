/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "AtlasExplorerPlugin.h"

#include <brayns/common/Log.h>
#include <brayns/network/entrypoint/EntrypointBuilder.h>

#include <io/NRRDLoader.h>

#include <network/entrypoints/GetAvailableAtlasUseCasesEntrypoint.h>
#include <network/entrypoints/VisualizeAtlasUseCaseEntrypoint.h>

AtlasExplorerPlugin::AtlasExplorerPlugin(brayns::PluginAPI &api)
    : _api(api)
{
}

void AtlasExplorerPlugin::onCreate()
{
    auto &registry = _api.getLoaderRegistry();
    registry.registerLoader(std::make_unique<NRRDLoader>());
}

void AtlasExplorerPlugin::registerEntrypoints(brayns::INetworkInterface &interface)
{
    auto &engine = _api.getEngine();
    auto &scene = engine.getScene();
    auto &models = scene.getModels();

    auto builder = brayns::EntrypointBuilder("Atlas Explorer", interface);

    builder.add<GetAvailableAtlasUseCasesEntrypoint>(models);
    builder.add<VisualizeAtlasUseCaseEntrypoint>(models);
}

extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(brayns::PluginAPI &api)
{
    brayns::Log::info("[AtlasExplorer] Loading Atlas Explorer plugin.");
    return std::make_unique<AtlasExplorerPlugin>(api);
}

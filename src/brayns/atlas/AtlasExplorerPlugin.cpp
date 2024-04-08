/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/network/entrypoint/EntrypointBuilder.h>
#include <brayns/core/utils/Log.h>

#include <brayns/atlas/io/NRRDLoader.h>

#include <brayns/atlas/network/entrypoints/GetAvailableAtlasUseCasesEntrypoint.h>
#include <brayns/atlas/network/entrypoints/VisualizeAtlasUseCaseEntrypoint.h>

namespace brayns
{
void loadAtlasExplorer(PluginAPI &api)
{
    auto name = "Atlas Explorer";

    auto &registry = api.getLoaderRegistry();
    auto loaders = LoaderRegistryBuilder(name, registry);

    loaders.add<NRRDLoader>();

    auto interface = api.getNetworkInterface();
    if (!interface)
    {
        return;
    }

    auto &engine = api.getEngine();
    auto &scene = engine.getScene();
    auto &models = scene.getModels();
    auto entrypoints = EntrypointBuilder(name, *interface);

    entrypoints.add<GetAvailableAtlasUseCasesEntrypoint>(models);
    entrypoints.add<VisualizeAtlasUseCaseEntrypoint>(models);
}
}

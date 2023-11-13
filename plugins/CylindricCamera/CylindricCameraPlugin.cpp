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

#include "CylindricCameraPlugin.h"

#include <brayns/network/entrypoint/EntrypointBuilder.h>
#include <brayns/utils/Log.h>

#include "camera/CylindricAdapter.h"
#include "network/entrypoints/CylindricCameraEntrypoint.h"

CylindricCameraPlugin::CylindricCameraPlugin(brayns::PluginAPI &api)
{
    auto &engine = api.getEngine();
    auto &factories = engine.getFactories();
    auto &cameras = factories.cameras;
    cameras.addType<Cylindric>();

    auto interface = api.getNetworkInterface();
    if (!interface)
    {
        return;
    }

    auto entrypoints = brayns::EntrypointBuilder("Cylindric Camera", *interface);

    entrypoints.add<SetCylindricCameraEntrypoint>(engine);
    entrypoints.add<GetCylindricCameraEntrypoint>(engine);
}

extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(brayns::PluginAPI &api)
{
    brayns::Log::info("[CC] Loading Cylindric Camera plugin.");
    return std::make_unique<CylindricCameraPlugin>(api);
}

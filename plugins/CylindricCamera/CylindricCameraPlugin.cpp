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

#include "CylindricCameraPlugin.h"

#include <brayns/common/Log.h>
#include <brayns/network/entrypoint/EntrypointBuilder.h>

#include "camera/CylindricCamera.h"
#include "camera/ospray/OsprayCylindricCamera.h"
#include "network/entrypoints/CylindricCameraEntrypoint.h"

CylindricCameraPlugin::CylindricCameraPlugin(brayns::PluginAPI &api)
    : _engine(api.getEngine())
{
}

void CylindricCameraPlugin::onCreate()
{
    ospray::Camera::registerType<OsprayCylindricCamera>(CylindricCamera::typeName.c_str());

    auto &cameraFactory = _engine.getCameraFactory();
    cameraFactory.registerType<CylindricCamera>();
}

void CylindricCameraPlugin::registerEntrypoints(brayns::INetworkInterface &interface)
{
    auto builder = brayns::EntrypointBuilder("Cylindric Camera", interface);
    builder.add<SetCylindricCameraEntrypoint>(_engine);
    builder.add<GetCylindricCameraEntrypoint>(_engine);
}

extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(brayns::PluginAPI &api)
{
    brayns::Log::info("[CC] Loading Cylindric Camera plugin.");
    return std::make_unique<CylindricCameraPlugin>(api);
}

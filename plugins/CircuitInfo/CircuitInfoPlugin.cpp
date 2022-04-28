/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrerou <nadir.romanguerrero@epfl.ch>
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

#include "CircuitInfoPlugin.h"

#include <brayns/common/Log.h>
#include <brayns/network/entrypoint/EntrypointBuilder.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <entrypoints/CIGetAfferentCellIdsEntrypoint.h>
#include <entrypoints/CIGetCellDataEntrypoint.h>
#include <entrypoints/CIGetCellIdsEntrypoint.h>
#include <entrypoints/CIGetCellIdsFromModelEntrypoint.h>
#include <entrypoints/CIGetEfferentCellIdsEntrypoint.h>
#include <entrypoints/CIGetProjectionEfferentCellIdsEntrypoint.h>
#include <entrypoints/CIGetProjectionsEntrypoint.h>
#include <entrypoints/CIGetReportInfoEntrypoint.h>
#include <entrypoints/CIGetReportsEntrypoint.h>
#include <entrypoints/CIGetSpikeReportInfoEntrypoint.h>
#include <entrypoints/CIGetTargetsEntrypoint.h>
#include <entrypoints/CIInfoEntrypoint.h>

void CircuitInfoPlugin::registerEntrypoints(brayns::INetworkInterface &interface)
{
    auto &engine = _api->getEngine();
    auto &scene = engine.getScene();
    auto &modelManager = scene.getModels();

    auto builder = brayns::EntrypointBuilder("Circuit Info", interface);

    builder.add<CIInfoEntrypoint>();
    builder.add<CIGetCellDataEntrypoint>();
    builder.add<CIGetCellIdsEntrypoint>();
    builder.add<CIGetCellIdsFromModelEntrypoint>(modelManager);
    builder.add<CIGetReportsEntrypoint>();
    builder.add<CIGetReportInfoEntrypoint>();
    builder.add<CIGetSpikeReportInfoEntrypoint>();
    builder.add<CIGetTargetsEntrypoint>();
    builder.add<CIGetAfferentCellIdsEntrypoint>();
    builder.add<CIGetEfferentCellIdsEntrypoint>();
    builder.add<CIGetProjectionsEntrypoint>();
    builder.add<CIGetProjectionEfferentCellIdsEntrypoint>();
}

extern "C" brayns::ExtensionPlugin *brayns_plugin_create(int /*argc*/, char ** /*argv*/)
{
    brayns::Log::info("[CI] Loading circuit info plugin");
    return new CircuitInfoPlugin();
}

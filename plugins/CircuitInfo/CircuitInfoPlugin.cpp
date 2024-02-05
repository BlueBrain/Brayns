/* Copyright 2019-2024 Blue Brain Project/EPFL
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
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/pluginapi/PluginAPI.h>

#include "CircuitInfoEntrypoints.h"

CircuitInfoPlugin::CircuitInfoPlugin()
    : brayns::ExtensionPlugin("Circuit Info")
{
}

void CircuitInfoPlugin::init()
{
    CircuitInfoEntrypoints::load(*this);
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    brayns::Log::info("[CI] Loading circuit info plugin");
    return new CircuitInfoPlugin();
}

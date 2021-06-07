/* Copyright (c) 2019, EPFL/Blue Brain Project
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

#include "CircuitRendererPlugin.h"
#include "Log.h"

#include <brayns/common/ActionInterface.h>
#include <brayns/common/propertymap/PropertyMap.h>
#include <brayns/engine/Engine.h>
#include <brayns/pluginapi/PluginAPI.h>

void _addTestRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering test renderer" << std::endl;
    brayns::PropertyMap properties;

    engine.addRendererType("test", properties);
}

CircuitRendererPlugin::CircuitRendererPlugin()
    : ExtensionPlugin()
{
}

CircuitRendererPlugin::~CircuitRendererPlugin() {}

void CircuitRendererPlugin::init()
{
    _addTestRenderer(_api->getEngine());
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit renderer plugin" << std::endl;
    return new CircuitRendererPlugin();
}

namespace ospray
{
extern "C" void ospray_init_module_braynsCircuitRenderer()
{
    PLUGIN_INFO << "Initializing circuit renderer module" << std::endl;
}
} // namespace ospray

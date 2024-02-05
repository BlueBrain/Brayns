/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <cyrille.favreau@epfl.ch>
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

#pragma once

#include <brayns/pluginapi/ExtensionPlugin.h>
#include <brayns/utils/DynamicLib.h>

#include <vector>

namespace brayns
{
/**
 */
class PluginManager
{
public:
    /**
     * @brief Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    PluginManager(int argc, const char** argv);

    /** Calls ExtensionPlugin::init in all loaded plugins */
    void initPlugins(PluginAPI* api);

    /** Destroys all plugins. */
    void destroyPlugins();

    /** Calls ExtensionPlugin::preRender in all loaded plugins */
    void preRender();

    /** Calls ExtensionPlugin::postRender in all loaded plugins */
    void postRender();

private:
    std::vector<DynamicLib> _libs;
    std::vector<std::unique_ptr<ExtensionPlugin>> _extensions;

    void _loadPlugin(const char* name, int argc, const char* argv[]);
};
} // namespace brayns

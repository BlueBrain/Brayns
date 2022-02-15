/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/network/plugin/NetworkManager.h>
#include <brayns/pluginapi/ExtensionPlugin.h>
#include <brayns/utils/DynamicLib.h>

#include <vector>

namespace brayns
{
/**
 * @brief Plugin manager to load and update plugins.
 *
 */
class PluginManager
{
public:
    /**
     * @brief Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    PluginManager(int argc, const char **argv);

    /**
     * @brief Load network engine if required and call init() on all plugins.
     *
     * @param api API access.
     */
    void initPlugins(PluginAPI *api);

    /**
     * @brief Destroy all plugins.
     *
     */
    void destroyPlugins();

    /**
     * @brief Call preRender() on all plugins.
     *
     */
    void preRender();

    /**
     * @brief Call postRender() on all plugins
     *
     */
    void postRender();

    /**
     * @brief Get the network manager plugin.
     *
     * @return NetworkManager* Network manager or null if disabled.
     */
    NetworkManager *getNetworkManager() const;

private:
    std::vector<DynamicLib> _libs;
    std::vector<std::unique_ptr<ExtensionPlugin>> _extensions;
    NetworkManager *_networkManager = nullptr;

    void _loadPlugin(const char *name, int argc, const char *argv[]);
};
} // namespace brayns

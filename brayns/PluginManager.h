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

#include <brayns/pluginapi/IPlugin.h>
#include <brayns/pluginapi/PluginAPI.h>
#include <brayns/utils/DynamicLib.h>

#include <memory>
#include <optional>
#include <vector>

namespace brayns
{
struct PluginRef
{
    std::optional<DynamicLib> library;
    std::unique_ptr<IPlugin> plugin;
};

/**
 * @brief Plugin manager to load and update plugins.
 *
 */
class PluginManager
{
public:
    /**
     * @brief Construct with API access
     *
     * @param api Brayns API interface.
     */
    PluginManager(PluginAPI &api);

    /**
     * @brief Load network engine if required and call onCreate() on plugins.
     *
     * @param api API access.
     */
    void loadPlugins();

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

private:
    PluginAPI &_api;
    std::vector<PluginRef> _plugins;
};
} // namespace brayns

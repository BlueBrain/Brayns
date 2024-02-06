/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#include "PluginManager.h"

#include <brayns/utils/Log.h>

namespace
{
class PluginLoader
{
public:
    static std::vector<brayns::Plugin> loadPlugins(brayns::PluginAPI &api)
    {
        std::vector<brayns::Plugin> plugins;
        auto &names = _getPluginNames(api);
        plugins.reserve(names.size());
        for (const auto &name : names)
        {
            auto plugin = _loadPlugin(api, name);
            plugins.push_back(std::move(plugin));
        }
        return plugins;
    }

private:
    using Factory = std::unique_ptr<brayns::IPlugin> (*)(brayns::PluginAPI &);

    static const std::vector<std::string> &_getPluginNames(brayns::PluginAPI &api)
    {
        auto &parameters = api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        return application.getPlugins();
    }

    static brayns::Plugin _loadPlugin(brayns::PluginAPI &api, const std::string &name)
    {
        brayns::Log::info("Loading plugin '{}'.", name);
        auto library = brayns::DynamicLib(name);
        auto factory = _getFactory(name, library);
        auto plugin = factory(api);
        brayns::Log::info("Plugin '{}' loaded.", name);
        return {std::move(library), std::move(plugin)};
    }

    static Factory _getFactory(const std::string &name, const brayns::DynamicLib &library)
    {
        auto address = library.getSymbolAddress("brayns_create_plugin");
        if (!address)
        {
            auto message = "Plugin library '" + name + "' has no symbols 'brayns_create_plugin'";
            throw std::runtime_error(message);
        }
        return reinterpret_cast<Factory>(address);
    }
};
} // namespace

namespace brayns
{
PluginManager::PluginManager(PluginAPI &api):
    _api(api)
{
}

void PluginManager::loadPlugins()
{
    _plugins = PluginLoader::loadPlugins(_api);
}

void PluginManager::destroyPlugins()
{
    _plugins.clear();
}
} // namespace brayns

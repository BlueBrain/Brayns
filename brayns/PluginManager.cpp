/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/Log.h>

#include <brayns/network/plugin/NetworkManager.h>

namespace
{
class PluginLoader
{
public:
    using Factory = std::unique_ptr<brayns::IPlugin> (*)(brayns::PluginAPI &);

    PluginLoader(brayns::PluginAPI &api)
        : _api(api)
    {
    }

    void loadPlugins(std::vector<brayns::PluginRef> &plugins)
    {
        auto names = _findPluginsToLoad();
        for (const auto &name : names)
        {
            auto plugin = _loadPlugin(name);
            plugins.push_back(std::move(plugin));
        }
    }

private:
    brayns::PluginAPI &_api;

    const std::vector<std::string> &_findPluginsToLoad()
    {
        auto &parameters = _api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        return application.getPlugins();
    }

    brayns::PluginRef _loadPlugin(const std::string &name)
    {
        brayns::Log::info("Loading plugin '{}'.", name);
        auto library = brayns::DynamicLib(name);
        auto plugin = _createPlugin(name, library);
        brayns::Log::info("Plugin '{}' loaded.", name);
        return {std::move(library), std::move(plugin)};
    }

    std::unique_ptr<brayns::IPlugin> _createPlugin(const std::string &name, const brayns::DynamicLib &library)
    {
        auto address = library.getSymbolAddress("brayns_create_plugin");
        if (!address)
        {
            auto message = "Plugin library '" + name + "' has no symbol 'brayns_create_plugin'";
            throw std::runtime_error(message);
        }
        auto factory = reinterpret_cast<Factory>(address);
        return factory(_api);
    }
};

class NetworkLoader
{
public:
    NetworkLoader(brayns::PluginAPI &api)
        : _api(api)
    {
    }

    bool isEnabled() const
    {
        auto &parameters = _api.getParametersManager();
        auto &networkParameters = parameters.getNetworkParameters();
        auto &uri = networkParameters.getUri();
        return !uri.empty();
    }

    brayns::NetworkManager &loadPlugin(std::vector<brayns::PluginRef> &plugins)
    {
        auto plugin = std::make_unique<brayns::NetworkManager>(_api);
        auto &ref = *plugin;
        plugins.push_back({std::nullopt, std::move(plugin)});
        return ref;
    }

    void start(brayns::NetworkManager &network, const std::vector<brayns::PluginRef> &plugins)
    {
        auto &interface = network.getInterface();
        for (const auto &[library, plugin] : plugins)
        {
            plugin->registerEntrypoints(interface);
        }
        network.start();
    }

private:
    brayns::PluginAPI &_api;
};

class PluginBuilder
{
public:
    PluginBuilder(brayns::PluginAPI &api)
        : _networkLoader(api)
        , _pluginLoader(api)
    {
    }

    std::vector<brayns::PluginRef> buildPlugins()
    {
        std::vector<brayns::PluginRef> plugins;
        if (!_networkLoader.isEnabled())
        {
            _createPlugins(plugins);
            return plugins;
        }
        _createNetworkAndPlugins(plugins);
        return plugins;
    }

private:
    NetworkLoader _networkLoader;
    PluginLoader _pluginLoader;

    void _createPlugins(std::vector<brayns::PluginRef> &plugins)
    {
        _pluginLoader.loadPlugins(plugins);
        for (const auto &[library, plugin] : plugins)
        {
            plugin->onCreate();
        }
    }

    void _createNetworkAndPlugins(std::vector<brayns::PluginRef> &plugins)
    {
        auto &network = _networkLoader.loadPlugin(plugins);
        _createPlugins(plugins);
        _networkLoader.start(network, plugins);
    }
};
} // namespace

namespace brayns
{
PluginManager::PluginManager(PluginAPI &api)
    : _api(api)
{
}

void PluginManager::loadPlugins()
{
    auto builder = PluginBuilder(_api);
    _plugins = builder.buildPlugins();
}

void PluginManager::destroyPlugins()
{
    _plugins.clear();
}

void PluginManager::preRender()
{
    for (const auto &[library, plugin] : _plugins)
    {
        plugin->onPreRender();
    }
}

void PluginManager::postRender()
{
    for (const auto &[library, plugin] : _plugins)
    {
        plugin->onPostRender();
    }
}
} // namespace brayns

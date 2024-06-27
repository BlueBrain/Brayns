/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#include "Brayns.h"

#include <string_view>
#include <thread>

#include <brayns/core/utils/Log.h>

#include <brayns/core/network/common/Clock.h>

#include "Version.h"

namespace
{
class LoggingStartup
{
public:
    static void run(brayns::ParametersManager &parameters)
    {
        _applyLevel(parameters);
        _logVersion();
        _logParameters(parameters);
    }

private:
    static void _applyLevel(brayns::ParametersManager &parameters)
    {
        auto &application = parameters.getApplicationParameters();
        auto level = application.getLogLevel();
        brayns::Log::setLevel(level);
    }

    static void _logVersion()
    {
        auto copyright = brayns::getCopyright();
        brayns::Log::info("{}.", copyright);
    }

    static void _logParameters(brayns::ParametersManager &parameters)
    {
        brayns::Log::info("Command line parameters values:");
        auto properties = parameters.getArgvProperties();
        for (const auto &property : properties)
        {
            auto &name = property.name;
            auto value = property.stringify();
            brayns::Log::info("'{}' = '{}'", name, value);
        }
    }
};

class NetworkStartup
{
public:
    static bool isEnabled(brayns::PluginAPI &api)
    {
        auto &manager = api.getParametersManager();
        auto &parameters = manager.getNetworkParameters();
        auto &uri = parameters.getUri();
        return !uri.empty();
    }
};
} // namespace

namespace brayns
{
Brayns::Brayns(int argc, const char **argv):
    _parametersManager(argc, argv),
    _engine(_parametersManager)
{
    LoggingStartup::run(_parametersManager);

    Log::info("Registering core loaders.");
    _loaderRegistry = CoreLoaderRegistry::create();

    if (NetworkStartup::isEnabled(*this))
    {
        Log::info("Initializing network manager.");
        _network = std::make_unique<NetworkManager>(*this);
    }
}

Brayns::~Brayns()
{
    _loaderRegistry = {};
}

void Brayns::commitAndRender()
{
    _engine.commitAndRender();
}

void Brayns::runAsService()
{
    if (!_network)
    {
        throw std::runtime_error("Trying to run a service without URI");
    }
    Log::info("Brayns service started.");
    _network->run();
}

Engine &Brayns::getEngine()
{
    return _engine;
}

ParametersManager &Brayns::getParametersManager()
{
    return _parametersManager;
}

LoaderRegistry &Brayns::getLoaderRegistry()
{
    return _loaderRegistry;
}

INetworkInterface *Brayns::getNetworkInterface()
{
    return _network.get();
}
} // namespace brayns

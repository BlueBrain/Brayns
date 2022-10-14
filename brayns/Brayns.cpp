/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <thread>

#include <brayns/Version.h>

#include <brayns/utils/Log.h>

#include <brayns/network/common/Clock.h>

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
        brayns::Log::info(
            "Brayns version {}.{}.{} ({}) Copyright (c) 2015-2022, EPFL/Blue Brain Project.",
            brayns::Version::getMajor(),
            brayns::Version::getMinor(),
            brayns::Version::getPatch(),
            brayns::Version::getCommitHash());
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

class RateLimiter
{
public:
    RateLimiter(brayns::Duration period)
        : _period(period)
    {
    }

    void wait()
    {
        auto now = brayns::Clock::now();
        auto delta = now - _lastCall;
        if (delta >= _period)
        {
            return;
        }
        std::this_thread::sleep_for(_period - delta);
        _lastCall = brayns::Clock::now();
    }

private:
    brayns::Duration _period;
    brayns::TimePoint _lastCall = brayns::Clock::now();
};
} // namespace

namespace brayns
{
Brayns::Brayns(int argc, const char **argv)
    : _parametersManager(argc, argv)
    , _engine(_parametersManager)
    , _pluginManager(*this)
{
    LoggingStartup::run(_parametersManager);

    Log::info("Register core loaders.");
    _loaderRegistry = LoaderRegistry::createWithCoreLoaders();

    if (NetworkStartup::isEnabled(*this))
    {
        Log::info("Initialize network manager.");
        _network = std::make_unique<NetworkManager>(*this);
    }

    Log::info("Load plugins.");
    _pluginManager.loadPlugins();

    if (_network)
    {
        Log::info("Start network manager.");
        _network->start();
    }
}

Brayns::~Brayns()
{
    if (_network)
    {
        _network->stop();
    }
    _loaderRegistry.clear();
    // make sure that plugin objects are removed first, as plugins are
    // destroyed before the engine, but plugin destruction still should have
    // a valid engine and _api (aka this object).
    _pluginManager.destroyPlugins();
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
    auto limiter = RateLimiter(std::chrono::milliseconds(1));
    while (_engine.isRunning())
    {
        _network->update();
        limiter.wait();
    }
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

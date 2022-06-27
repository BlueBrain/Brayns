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

#include <brayns/Brayns.h>
#include <brayns/Version.h>

#include <brayns/common/Log.h>

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
} // namespace

namespace brayns
{
SystemPluginAPI::SystemPluginAPI(ParametersManager &paramManager, Engine &engine, LoaderRegistry &loadRegistry)
    : _paramManager(paramManager)
    , _engine(engine)
    , _loadRegistry(loadRegistry)
{
}

Engine &SystemPluginAPI::getEngine()
{
    return _engine;
}

ParametersManager &SystemPluginAPI::getParametersManager()
{
    return _paramManager;
}

LoaderRegistry &SystemPluginAPI::getLoaderRegistry()
{
    return _loadRegistry;
}

// -----------------------------------------------------------------------------

Brayns::Brayns(int argc, const char **argv)
    : _parametersManager(argc, argv)
    , _engine(_parametersManager)
    , _pluginAPI(_parametersManager, _engine, _loaderRegistry)
    , _pluginManager(_pluginAPI)
{
    LoggingStartup::run(_parametersManager);
    _loaderRegistry = LoaderRegistry::createWithCoreLoaders();
    _pluginManager.loadPlugins();
}

Brayns::~Brayns()
{
    _loaderRegistry.clear();
    // make sure that plugin objects are removed first, as plugins are
    // destroyed before the engine, but plugin destruction still should have
    // a valid engine and _api (aka this object).
    _pluginManager.destroyPlugins();
}

bool Brayns::commitAndRender()
{
    // Pre render plugins
    _pluginManager.preRender();

    // Pre render engine
    _engine.preRender();

    // Commit any change to the engine (scene, camera, renderer, parameters, ...)
    _engine.commit();

    // Render new frame, if needed
    _engine.render();

    // The parameters are modified on pluginManager.preRender, and processed on engine.preRender and engine.commit
    _parametersManager.resetModified();

    // Post render engine
    _engine.postRender();

    // Post render plugins
    _pluginManager.postRender();

    return _engine.isRunning();
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
} // namespace brayns

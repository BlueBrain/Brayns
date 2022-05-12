/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/PluginManager.h>
#include <brayns/common/parameters/ParametersManager.h>
#include <brayns/engine/Engine.h>
#include <brayns/io/LoaderRegistry.h>

namespace brayns
{
/**
 * @brief The SystemPluginAPI class is an imlementation of the PluginAPI, a class designed to give access
 * to the system resources to the plugins
 */
class SystemPluginAPI : public PluginAPI
{
public:
    SystemPluginAPI(
        ParametersManager &paramManager,
        PluginManager &pluginManager,
        Engine &engine,
        LoaderRegistry &loadRegistry);

    Engine &getEngine() final;

    ParametersManager &getParametersManager() final;

    LoaderRegistry &getLoaderRegistry() final;

private:
    ParametersManager &_paramManager;
    PluginManager &_pluginManager;
    Engine &_engine;
    LoaderRegistry &_loadRegistry;
};

/**
 * @brief The Brayns class is the entry point to the system, which initializes
 * all the necessary componenets to run the render engine and add plugins to the
 * framework
 */
class Brayns
{
public:
    Brayns(int argc = 0, const char **argv = nullptr);

    Brayns(const Brayns &) = delete;
    Brayns &operator=(const Brayns &) = delete;

    Brayns(Brayns &&) = default;
    Brayns &operator=(Brayns &&) = default;

    ~Brayns();

    /**
     * @brief commit Processes incomming client requests and makes the data available to the rendering
     * engine, then proceeds to render a new frame (if needed).
     * Returns wether the system is still running or not
     */
    bool commitAndRender();

    /**
     * @brief Get engine.
     *
     * @return Engine& Engine.
     */
    Engine &getEngine();

    /**
     * @brief Get parameters.
     *
     * @return ParametersManager& Brayns parameters.
     */
    ParametersManager &getParametersManager();

    /**
     * @brief Gives access to the loaders registry
     *
     * @return LoaderRegistry& All available loaders.
     */
    LoaderRegistry &getLoaderRegistry();

private:
    ParametersManager _parametersManager;
    PluginManager _pluginManager;
    Engine _engine;
    LoaderRegistry _loaderRegistry;
    SystemPluginAPI _pluginAPI;
};
} // namespace brayns

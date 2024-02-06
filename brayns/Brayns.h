/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <memory>

#include <brayns/PluginManager.h>
#include <brayns/engine/core/Engine.h>
#include <brayns/io/LoaderRegistry.h>
#include <brayns/network/NetworkManager.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
/**
 * @brief The Brayns class is the entry point to the system, which initializes
 * all the necessary componenets to run the render engine and add plugins to the
 * framework
 */
class Brayns : public PluginAPI
{
public:
    explicit Brayns(int argc = 0, const char **argv = nullptr);

    Brayns(const Brayns &) = delete;
    Brayns &operator=(const Brayns &) = delete;

    Brayns(Brayns &&) = delete;
    Brayns &operator=(Brayns &&) = delete;

    ~Brayns();

    /**
     * @brief commit Processes incomming client requests and makes the data available to the rendering
     * engine, then proceeds to render a new frame (if needed).
     */
    void commitAndRender();

    /**
     * @brief Poll socket while engine is running.
     *
     */
    void runAsService();

    /**
     * @brief Get parameters.
     *
     * @return ParametersManager& Brayns parameters.
     */
    virtual ParametersManager &getParametersManager() override;

    /**
     * @brief Gives access to the loaders registry
     *
     * @return LoaderRegistry& All available loaders.
     */
    virtual LoaderRegistry &getLoaderRegistry() override;

    /**
     * @brief Get engine.
     *
     * @return Engine& Engine.
     */
    virtual Engine &getEngine() override;

    /**
     * @brief Get network interface if enabled.
     *
     * @return INetworkInterface* Network interface, can be null.
     */
    virtual INetworkInterface *getNetworkInterface() override;

private:
    ParametersManager _parametersManager;
    LoaderRegistry _loaderRegistry;
    Engine _engine;
    PluginManager _pluginManager;
    std::unique_ptr<NetworkManager> _network;
};
} // namespace brayns

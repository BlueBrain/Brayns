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

#pragma once

#include <brayns/engine/core/Engine.h>
#include <brayns/io/LoaderRegistry.h>
#include <brayns/network/INetworkInterface.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
/**
 * @brief Expose Brayns functionalities to plugins.
 *
 */
class PluginAPI
{
public:
    virtual ~PluginAPI() = default;

    /**
     * @brief Get the parameters.
     *
     * @return ParametersManager& Parameters.
     */
    virtual ParametersManager &getParametersManager() = 0;

    /**
     * @brief Get the available loaders.
     *
     * @return LoaderRegistry& Loaders.
     */
    virtual LoaderRegistry &getLoaderRegistry() = 0;

    /**
     * @brief Get the engine.
     *
     * @return Engine& Engine.
     */
    virtual Engine &getEngine() = 0;

    /**
     * @brief Get the network interface if enabled.
     *
     * @return INetworkInterface& Network interface.
     */
    virtual INetworkInterface *getNetworkInterface() = 0;
};
} // namespace brayns

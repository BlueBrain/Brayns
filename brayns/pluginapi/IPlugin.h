/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/network/INetworkInterface.h>

namespace brayns
{
/**
 * Interface Brayns plugins have to implement.
 *
 * For a plugin to be loaded dynamically at runtime, the following function
 * must be available in the library:
 *
 * @code
 * extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(PluginAPI &api)
 * @endcode
 *
 * It receives a reference to Brayns API to access core data (engine,
 * parameters, loaders) and must return an instance of the plugin.
 *
 * Brayns will monitor the plugin using this interface.
 *
 */
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    /**
     * @brief Called once when everything is initialized.
     *
     * Called before all other methods.
     *
     */
    virtual void onCreate()
    {
    }

    /**
     * @brief Called once if the network plugin is enabled.
     *
     * @param interface Network access.
     */
    virtual void registerEntrypoints(INetworkInterface &interface)
    {
        (void)interface;
    }
};
} // namespace brayns

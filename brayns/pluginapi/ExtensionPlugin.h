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

#include <brayns/network/interface/INetworkInterface.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
/**
 * Defines the abstract representation of an extension plug-in. What we mean by
 * extension is a set a functionalities that are not provided by the core of the
 * application. For example, exposing a REST interface via HTTP, or streaming
 * images to an distant display.
 *
 * For a plugin to be loaded dynamically at runtime, the following function
 * must be available in the library:
 *
 * @code
 * extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int argc, const
 * char** argv)
 * @endcode
 *
 * It must return the instance of the plugin, and from hereon Brayns owns the
 * plugin and calls preRender() and postRender() accordingly.
 * In the shutdown sequence of Brayns, the plugin will be destructed properly.
 */
class ExtensionPlugin
{
public:
    virtual ~ExtensionPlugin() = default;

    /**
     * @brief Called once when the engine is created.
     */
    virtual void init()
    {
    }

    /**
     * @brief Called once when the network is initialized.
     *
     * @param interface Network access.
     */
    virtual void registerEntrypoints(INetworkInterface &interface)
    {
    }

    /**
     * @brief Called on each update before trying to call render().
     *
     */
    virtual void preRender()
    {
    }

    /**
     * @brief Called once a render() is effectively called.
     *
     */
    virtual void postRender()
    {
    }

protected:
    friend class PluginManager;

    PluginAPI *_api = nullptr;
};
} // namespace brayns

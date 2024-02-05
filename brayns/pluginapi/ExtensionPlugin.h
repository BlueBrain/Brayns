/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <cassert>

#include <brayns/network/entrypoint/EntrypointRef.h>
#include <brayns/network/interface/ActionInterface.h>
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
    ExtensionPlugin(std::string name = {})
        : _name(std::move(name))
    {
    }

    virtual ~ExtensionPlugin() = default;

    /**
     * Called from Brayns::Brayns right after the engine has been created
     */
    virtual void init() {}

    /**
     * Called from Brayns::preRender() to prepare the engine based on the
     * plugins' need for an upcoming render().
     */
    virtual void preRender() {}

    /** Called from Brayns::postRender() after render() has finished. */
    virtual void postRender() {}

    /**
     * @brief Add an entrypoint of given type built using given args.
     *
     * Must be called in init() method.
     *
     * If an action interface is registered, it will be used to register the
     * entrypoint, otherwise nothing will be done.
     *
     * @tparam T Entrypoint type.
     * @tparam Args Entrypoint construction arguments types.
     * @param args Entrypoint construction arguments.
     */
    template <typename T, typename... Args>
    void add(Args&&... args)
    {
        assert(_api);
        auto interface = _api->getActionInterface();
        if (!interface)
        {
            return;
        }
        auto entrypoint = EntrypointRef::create<T>(std::forward<Args>(args)...);
        entrypoint.setPlugin(_name);
        interface->addEntrypoint(std::move(entrypoint));
    }

protected:
    friend class PluginManager;

    PluginAPI* _api = nullptr;

private:
    std::string _name;
};
} // namespace brayns

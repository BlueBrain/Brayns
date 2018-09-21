/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#ifndef EXTENSIONPLUGIN_H
#define EXTENSIONPLUGIN_H

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
 * extern "C" brayns::ExtensionPlugin* brayns_plugin_create(brayns::PluginAPI*,
 * int argc, char** argv)
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
     * Called from Brayns::preRender() to prepare the engine based on the
     * plugins' need for an upcoming render().
     */
    virtual void preRender() {}
    /** Called from Brayns::postRender() after render() has finished. */
    virtual void postRender() {}
};
} // namespace brayns

#endif

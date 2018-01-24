/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef EXTENSIONPLUGINFACTORY_H
#define EXTENSIONPLUGINFACTORY_H

#include <brayns/common/types.h>

namespace brayns
{
/** Manages plug-ins such as Deflect Streamer or HTTP server
 */
class ExtensionPluginFactory
{
public:
    /**
        Constructs the object and initializes default plugins according to
        application parameters.
    */
    ExtensionPluginFactory(ParametersManager& parametersManager);

    ~ExtensionPluginFactory();

    /**
       Registers a new plug-in
       @param plugin Plug-in to register
     */
    void add(ExtensionPluginPtr plugin);

    /**
       Unregisters an existing plug-in
       @param plugin Plug-in to unregister
     */
    void remove(ExtensionPluginPtr plugin);

    /**
       Unregisters all registered plug-ins
     */
    void clear();

    /**
       Executes code specific to every registered plugin
     */
    void execute(EnginePtr engine, KeyboardHandler& keyboardHandler,
                 AbstractManipulator& cameraManipulator);

private:
    ExtensionPlugins _plugins;
};
}
#endif // EXTENSIONPLUGINFACTORY_H

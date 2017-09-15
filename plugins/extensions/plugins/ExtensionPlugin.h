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

#ifndef EXTENSIONPLUGIN_H
#define EXTENSIONPLUGIN_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{
/**
   Defines the abstract representation of a extension plug-in. What we mean by
   extension is a set a functionalities that are not provided by the core of
   the application. For example, exposing a REST interface via HTTP, or
   streaming images to an distant display.
 */
class ExtensionPlugin
{
public:
    virtual ~ExtensionPlugin() {}
    /**
     * Executes the core functionnalities of the plugin
     * @return true if other plugins are allowed to continue execution or false
     *         if control shall be returned to Brayns main loop for e.g.
     *         rendering a new frame.
     */
    BRAYNS_API virtual bool run(EnginePtr _engine,
                                KeyboardHandler& keyboardHandler,
                                AbstractManipulator& cameraManipulator) = 0;

protected:
    ExtensionPlugin();
};
}
#endif // EXTENSIONPLUGIN_H

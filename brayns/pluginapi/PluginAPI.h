/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/types.h>

namespace brayns
{
/** The API that plugins can use to interact with Brayns. */
class PluginAPI
{
public:
    virtual ~PluginAPI() = default;

    /** @return access to the scene of Brayns. */
    virtual Scene& getScene() = 0;

    /** @return access to the parameters of Brayns. */
    virtual ParametersManager& getParametersManager() = 0;

    /** @return access to the action interface of Brayns. */
    virtual ActionInterface* getActionInterface() = 0;

    /** @return access to the keyboard handler of Brayns. */
    virtual KeyboardHandler& getKeyboardHandler() = 0;

    /** @return access to the camera manipulator of Brayns. */
    virtual AbstractManipulator& getCameraManipulator() = 0;

    /** @return access to the camera of Brayns. */
    virtual Camera& getCamera() = 0;

    /** @return access to the renderer of Brayns. */
    virtual Renderer& getRenderer() = 0;

    /** Triggers a new preRender() and potentially render() and postRender(). */
    virtual void triggerRender() = 0;
};
} // namespace brayns

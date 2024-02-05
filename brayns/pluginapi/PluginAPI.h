/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/io/LoaderRegistry.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
class ActionInterface;

/** The API that plugins can use to interact with Brayns. */
class PluginAPI
{
public:
    virtual ~PluginAPI() = default;

    virtual Engine& getEngine() = 0;

    /** @return access to the scene of Brayns. */
    virtual Scene& getScene() = 0;

    /** @return access to the parameters of Brayns. */
    virtual ParametersManager& getParametersManager() = 0;

    /** @return access to the action interface of Brayns. */
    virtual ActionInterface* getActionInterface() = 0;

    /** @return access to the camera of Brayns. */
    virtual Camera& getCamera() = 0;

    /** @return access to the renderer of Brayns. */
    virtual Renderer& getRenderer() = 0;

    /**
     * @brief getLoaderRegistry gives access to the loader registry
     */
    virtual LoaderRegistry& getLoaderRegistry() = 0;

    /** Triggers a new preRender() and potentially render() and postRender(). */
    virtual void triggerRender() = 0;

    /** Set the action interface to be used by Brayns main loop. */
    virtual void setActionInterface(
        const std::shared_ptr<ActionInterface>& interface) = 0;
};
} // namespace brayns

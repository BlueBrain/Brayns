/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/engine/camera/Camera.h>
#include <brayns/engine/framebuffer/Framebuffer.h>
#include <brayns/engine/json/EngineFactories.h>
#include <brayns/engine/renderer/Renderer.h>
#include <brayns/engine/scene/Scene.h>
#include <brayns/parameters/ParametersManager.h>

#include <ospray/ospray_cpp/Device.h>

namespace brayns
{
/**
 * @brief The Engine class handles, and gives access to, the system resources for rendering. These resources are the
 * one used to stream frames to the clients, and serve as base objects when requesting snapshots ore export frames
 * tasks, if no custom ones are specified for such tasks
 */
class Engine
{
public:
    /**
     * @brief Initializes Ospray and register core engine objects available for use (cameras, renderers, materials
     * and lights)
     */
    explicit Engine(ParametersManager &parameters);

    /**
     * @brief Ensures that all the system data is updated on the Ospray rendered backend to ensure the
     * correct frame rendering. Called before render()
     */
    void commit();

    /**
     * @brief Render a new image if needed.
     *
     */
    void commitAndRender();

    /**
     * @brief Returns the system's Scene object.
     */
    Scene &getScene();

    /**
     * @brief Returns the system's Framebuffer object
     */
    Framebuffer &getFramebuffer() noexcept;

    /**
     * @brief Returns the system's current Camera object
     */
    Camera &getCamera() noexcept;

    /**
     * @brief Returns the system's current Renderer object
     */
    Renderer &getRenderer() noexcept;

    /**
     * @brief Return the manager of engine object factories.
     * @return EngineFactories&
     */
    EngineFactories &getFactories() noexcept;

    /**
     * @brief Returns the system parameters manager
     */
    const ParametersManager &getParametersManager() const noexcept;

private:
    /**
     * @brief Attempts to render a frame (if accumulation hasnt finish integrating the current frame and/or the
     * contents of the engine have changed).
     */
    void _render();

private:
    /**
     * @brief In charge of managing the lifetime of ospray modules
     */
    class OsprayModuleHandler
    {
    public:
        OsprayModuleHandler();
        ~OsprayModuleHandler();
    };

private:
    // Global system parameters used to read when updating the backend during commit() and render()
    ParametersManager &_params;

    OsprayModuleHandler _moduleHandler;
    ospray::cpp::Device _osprayDevice;

    // System objects
    Framebuffer _frameBuffer;
    Scene _scene;
    Camera _camera;
    Renderer _renderer;

    EngineFactories _factories;
};
} // namespace brayns

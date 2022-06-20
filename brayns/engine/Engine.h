/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/parameters/ParametersManager.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

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
    Engine(ParametersManager &parameters);

    ~Engine();

    /**
     * @brief Called before commit() and render()
     */
    void preRender();

    /**
     * @brief Ensures that all the system data is updated on the Ospray rendered backend to ensure the
     * correct frame rendering. Called before render()
     */
    void commit();

    /**
     * @brief Attempts to render a frame (if accumulation hasnt finish integrating the current frame and/or the
     * contents of the engine have changed). It has built-in FPS limiter.
     */
    void render();

    /**
     * @brief Called after render()
     */
    void postRender();

    /**
     * @brief Returns the system's Scene object.
     */
    Scene &getScene();

    /**
     * @brief Returns the system's FrameBuffer object
     */
    FrameBuffer &getFrameBuffer() noexcept;

    /**
     * @brief Sets a new system Camera to use
     */
    void setCamera(std::unique_ptr<Camera> camera) noexcept;

    /**
     * @brief Returns the system's current Camera object
     */
    Camera &getCamera() noexcept;

    /**
     * @brief Sets a new system Renderer to use
     */
    void setRenderer(std::unique_ptr<Renderer> renderer) noexcept;

    /**
     * @brief Returns the system's current Renderer object
     */
    Renderer &getRenderer() noexcept;

    /**
     * @brief Sets wether the engine should keep running or not
     */
    void setRunning(bool running) noexcept;

    /**
     * @brief Returns wether the engine is running or not
     */
    bool isRunning() const noexcept;

    /**
     * @brief Returns the system parameters manager
     */
    const ParametersManager &getParametersManager() const noexcept;

private:
    // Global system parameters used to read when updating the backend during commit() and render()
    ParametersManager &_params;

    ospray::cpp::Device _osprayDevice;

    // System objects
    FrameBuffer _frameBuffer;
    Scene _scene;
    std::unique_ptr<Camera> _camera;
    std::unique_ptr<Renderer> _renderer;

    // Run flag
    bool _keepRunning{true};
};
} // namespace brayns

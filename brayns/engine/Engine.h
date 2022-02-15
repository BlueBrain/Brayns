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

#include <brayns/common/PixelFormat.h>
#include <brayns/common/Statistics.h>
#include <brayns/common/propertymap/PropertyMap.h>

#include <brayns/engine/Camera.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/ParametersManager.h>

#include <functional>

namespace brayns
{
/**
 * Abstract implementation of the ray-tracing engine. What we call the
 * ray-tracing engine is a 3rd party acceleration library, typically OSPRay,
 * that provides hardware acceleration.
 * An engine holds a native implementation of a scene, a camera, a frame buffer
 * and of one or several renderers according to the capatilities of the
 * acceleration library.
 */
class Engine
{
public:
    /** @name API for engine-specific code */
    //@{
    /**
     * Commits changes to the engine. This include scene, camera and renderer
     * modifications
     */
    virtual void commit();

    /** Executes engine specific pre-render operations */
    virtual void preRender();

    /** Executes engine specific post-render operations */
    virtual void postRender();

    /** @return the minimum frame size in pixels supported by this engine. */
    virtual Vector2ui getMinimumFrameSize() const = 0;

    /** Factory method to create an engine-specific framebuffer. */
    virtual FrameBufferPtr
        createFrameBuffer(const std::string &name, const Vector2ui &frameSize, PixelFormat frameBufferFormat) const = 0;

    /** Factory method to create an engine-specific scene. */
    virtual ScenePtr createScene(AnimationParameters &animationParameters, VolumeParameters &volumeParameters)
        const = 0;

    /** Factory method to create an engine-specific camera. */
    virtual CameraPtr createCamera() const = 0;

    /** Factory method to create an engine-specific renderer. */
    virtual RendererPtr createRenderer(
        const AnimationParameters &animationParameters,
        const RenderingParameters &renderingParameters) const = 0;
    //@}

    /**
     * @brief Engine constructor
     * @param parametersManager holds all engine parameters (geometry,
     * rendering, etc)
     */
    explicit Engine(ParametersManager &parametersManager);

    virtual ~Engine() = default;

    /** Renders the current scene and populates the frame buffer accordingly */
    void render();

    /** Gets the scene */
    Scene &getScene();

    /** Gets the frame buffer */
    FrameBuffer &getFrameBuffer();

    /** Gets the camera */
    const Camera &getCamera() const;

    Camera &getCamera();

    /** Gets the renderer */
    Renderer &getRenderer();

    /**
     * Keep continue to run the engine, aka the user did not request to stop
     * rendering.
     */
    void setKeepRunning(bool keepRunning);

    /**
     * @return true if the user wants to continue rendering, false otherwise.
     */
    bool getKeepRunning() const;

    Statistics &getStatistics();

    /**
     * @return true if render() calls shall be continued, based on current
     *         accumulation settings.
     * @sa RenderingParameters::setMaxAccumFrames
     */
    bool continueRendering() const;

    const ParametersManager &getParametersManager() const;

    ParametersManager &getParametersManager();

    /**
     * Add the given frame buffer to the list of buffers that shall be filled
     * during rendering.
     */
    void addFrameBuffer(FrameBufferPtr frameBuffer);

    /**
     * Remove the given frame buffer from the list of buffers that are filled
     * during rendering.
     */
    void removeFrameBuffer(FrameBufferPtr frameBuffer);

    /** @return all registered frame buffers that are used during rendering. */
    const std::vector<FrameBufferPtr> &getFrameBuffers() const;

    /** @internal Clear all frame buffers. */
    void clearFrameBuffers();

    /** @internal resetModified() all frame buffers. */
    void resetFrameBuffers();

    /**
     * Add a new renderer type with optional properties. The renderer
     * registration for a concrete engine is specific to the actual engine, e.g.
     * OSP_REGISTER_RENDERER for OSPRay.
     */
    void addRendererType(const std::string &name, const PropertyMap &properties = {});

    /**
     * Add a new camera type with optional properties. The camera registration
     * for a concrete engine is specific to the actual engine, e.g.
     * OSP_REGISTER_CAMERA for OSPRay.
     */
    void addCameraType(const std::string &name, const PropertyMap &properties = {});

private:
    bool mustRender();

protected:
    ParametersManager &_parametersManager;
    ScenePtr _scene;
    CameraPtr _camera;
    RendererPtr _renderer;
    std::vector<FrameBufferPtr> _frameBuffers;
    Statistics _statistics;

    bool _keepRunning{true};
};
} // namespace brayns

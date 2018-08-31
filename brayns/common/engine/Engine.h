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

#ifndef ENGINE_H
#define ENGINE_H

#include <brayns/common/Statistics.h>

#include <functional>

namespace brayns
{
/**
 * Abstract implementation of the ray-tracing engine. What we call the
 * ray-tracing engine is a 3rd party acceleration library, typically OSPRay,
 * Optix or FireRays, that provides hardware acceleration.
 * An engine holds a native implementation of a scene, a camera, a frame buffer
 * and of one or several renderers according to the capatilities of the
 * acceleration library.
 */
class Engine
{
public:
    /**
     * @brief Engine constructor
     * @param parametersManager holds all engine parameters (geometry,
     * rendering, etc)
     */
    explicit Engine(ParametersManager& parametersManager);
    virtual ~Engine() = default;

    /** @return the name of the engine */
    virtual EngineType name() const = 0;

    /**
     * Commits changes to the engine. This include scene, camera and renderer
     * modifications
     */
    virtual void commit();

    /** Renders the current scene and populates the frame buffer accordingly */
    void render();
    /** Executes engine specific pre-render operations */
    virtual void preRender() {}
    /** Executes engine specific post-render operations */
    virtual void postRender();
    /** Gets the scene */
    Scene& getScene() { return *_scene; }
    auto getScenePtr() { return _scene; }
    /** Gets the frame buffer */
    FrameBuffer& getFrameBuffer() { return *_frameBuffer; }
    /** Gets the camera */
    const Camera& getCamera() const { return *_camera; }
    Camera& getCamera() { return *_camera; }
    /** Gets the renderer */
    Renderer& getRenderer();
    /**
       Reshapes the current frame buffers
       @param frameSize New size for the buffers
    */
    void reshape(const Vector2ui& frameSize);

    /**
       Sets initial camera position for the scene handled by the engine
    */
    void setDefaultCamera();

    /**
       Initializes materials for the current scene
       @param colorMap Predefined color map
    */
    void initializeMaterials(
        MaterialsColorMap colorMap = MaterialsColorMap::none);

    /**
     * Callback when a new frame shall be triggered. Currently called by event
     * plugins Deflect and Rockets.
     */
    std::function<void()> triggerRender{[] {}};

    /**
     * Adapts the size of the frame buffer according to camera
     * requirements. Typically, in case of 3D stereo vision, the frame buffer
     * width has to be an even number.
     * Can be overridden by the engine implementation for specific requirements.
     *
     * @param size New size of the frame buffer
     * @return Size that matches the camera requirements
     */
    virtual Vector2ui getSupportedFrameSize(const Vector2ui& size) const = 0;

    /** @return the minimum frame size in pixels supported by this engine. */
    virtual Vector2ui getMinimumFrameSize() const = 0;

    /**
     * @return true if for "--module deflect" the DeflectPixelOp was
     *         successfully loaded. Only supported for the OSPRay engine.
     */
    virtual bool haveDeflectPixelOp() const { return false; }
    /**
     * Keep continue to run the engine, aka the user did not request to stop
     * rendering.
     */
    void setKeepRunning(bool keepRunning) { _keepRunning = keepRunning; }
    /**
     * @return true if the user wants to continue rendering, false otherwise.
     */
    bool getKeepRunning() const { return _keepRunning; }
    Statistics& getStatistics() { return _statistics; }
    /**
     * @return true if render() calls shall be continued, based on current
     *         accumulation settings.
     * @sa RenderingParameters::setMaxAccumFrames
     */
    bool continueRendering() const;

    /** Factory method to create an engine-specific framebuffer. */
    virtual FrameBufferPtr createFrameBuffer(
        const Vector2ui& frameSize, FrameBufferFormat frameBufferFormat,
        bool accumulation) const = 0;

    virtual ScenePtr createScene(
        ParametersManager& parametersManager) const = 0;

    /** Factory method to create an engine-specific camera. */
    virtual CameraPtr createCamera() const = 0;

    virtual RendererPtr createRenderer(
        const AnimationParameters& animationParameters,
        const RenderingParameters& renderingParameters) const = 0;

    auto& getParametersManager() { return _parametersManager; }
protected:
    void _render(const RenderInput& renderInput, RenderOutput& renderOutput);
    void _render();

    void _writeFrameToFile();

    ParametersManager& _parametersManager;
    ScenePtr _scene;
    CameraPtr _camera;
    RendererPtr _renderer;
    Vector2i _frameSize;
    FrameBufferPtr _frameBuffer;
    Statistics _statistics;

    bool _keepRunning{true};
};
}

#endif // ENGINE_H

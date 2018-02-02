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

#ifndef ENGINE_H
#define ENGINE_H

#include <brayns/common/Statistics.h>

#include <mutex>

namespace brayns
{
struct SnapshotParams
{
    int samplesPerPixel{1};
    Vector2ui size;
    std::string format; // ImageMagick formats apply
    size_t quality{100};
};

/**
 * Abstract implementation of the ray-tracing engine. What we call the
 * ray-tracing engine is a 3rd party acceleration library, typically OSPRay,
 * Optix or FireRays, that provides hardware acceleration.
 * An engine holds a native implementation of a scene, a camera, a frame buffer
 * and of one or serveral renderers according to the capatilities of the
 * acceleration library.
 */
class Engine
{
public:
    /**
     * @brief Engine contructor
     * @param parametersManager holds all engine parameters (geometry,
     * rendering, etc)
     */
    explicit Engine(ParametersManager& parametersManager);
    virtual ~Engine();

    /**
       @return the name of the engine
    */
    virtual EngineType name() const = 0;

    /**
       Commits changes to the engine. This include scene and camera
       modifications
    */
    virtual void commit() = 0;

    /** Renders the current scene and populates the frame buffer accordingly */
    virtual void render();
    /** Executes engine specific pre-render operations */
    virtual void preRender() {}
    /** Executes engine specific post-render operations */
    virtual void postRender() {}
    /** Gets the scene */
    Scene& getScene() { return *_scene; }
    /** Gets the frame buffer */
    FrameBuffer& getFrameBuffer() { return *_frameBuffer; }
    /** Gets the camera */
    const Camera& getCamera() const { return *_camera; }
    Camera& getCamera() { return *_camera; }
    /** Gets the renderer */
    Renderer& getRenderer();
    /** Active renderer */
    void setActiveRenderer(const RendererType renderer);
    RendererType getActiveRenderer() { return _activeRenderer; }
    /**
       Reshapes the current frame buffers
       @param frameSize New size for the buffers

       @todo Must be removed and held by the render method above
    */
    void reshape(const Vector2ui& frameSize);

    /**
       Sets default camera according to scene bounding box
    */
    void setDefaultCamera();

    /**
       Sets default epsilon to scene bounding box
    */
    void setDefaultEpsilon();

    /**
       Initializes materials for the current scene
       @param colorMap Predefined color map
    */
    void initializeMaterials(
        MaterialsColorMap colorMap = MaterialsColorMap::none);

    /**
     * Unloads the current scene and loads and builds a new scene according to
     * datasource parameters. The execution will be asynchronous if
     * getSynchronousMode() is false.
     */
    std::function<void()> buildScene;

    /**
     * @brief resets frame number
     */
    void resetFrameNumber();

    /**
     * @returns the current frame number
     */
    size_t getFrameNumber() const { return _frameNumber; }
    /**
     * Adapts the size of the frame buffer according to camera
     * requirements. Typically, in case of 3D stereo vision, the frame buffer
     * width has to be an even number.
     * Can be overridden by the engine implementation for specific requirements.
     *
     * @param size New size of the frame buffer
     * @return Size that matches the camera requirements
     */
    virtual Vector2ui getSupportedFrameSize(const Vector2ui& size);

    /** @return the minimum frame size in pixels supported by this engine. */
    virtual Vector2ui getMinimumFrameSize() const = 0;

    struct Progress : public BaseObject
    {
        std::string operation;
        float amount{0.f};
        mutable std::mutex mutex;

        template <typename T>
        void updateValue(T& member, const T& newValue)
        {
            _updateValue(member, newValue);
        }
    };

    /** @return the current progress of the engine */
    const Progress& getProgress() const { return _progress; }
    Progress& getProgress() { return _progress; }
    /** Set the last operation processed by the engine. */
    void setLastOperation(const std::string& lastOperation)
    {
        _progress.updateValue(_progress.operation, lastOperation);
    }

    /**
     * Set the last normalized progress value (0..1) for any current operation.
     */
    void setLastProgress(const float lastProgress)
    {
        _progress.updateValue(_progress.amount, lastProgress);
    }

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
    /**
     * @return true if the engine is ready to render and receive updates, false
     *         if data loading is in progress.
     */
    bool isReady() const { return _isReady; }
    /** @internal */
    void setReady(const bool isReady_) { _isReady = isReady_; }
    Statistics& getStatistics() { return _statistics; }
    /**
     * Render a snapshot with the given parameters. Result is available in
     * framebuffer.
     *
     * @note not threadsafe and not safe vs render()
     * @param params the snapshot parameter to take
     * @throws std::runtime_error if engine is not ready or snapshot creation
     *         failed
     */
    void snapshot(const SnapshotParams& params);

protected:
    void _render(const RenderInput& renderInput, RenderOutput& renderOutput);
    void _render();

    ParametersManager& _parametersManager;
    ScenePtr _scene;
    CameraPtr _camera;
    RendererType _activeRenderer;
    RendererMap _renderers;
    Vector2i _frameSize;
    FrameBufferPtr _frameBuffer;
    Statistics _statistics;

    size_t _frameNumber;
    Progress _progress;
    bool _keepRunning{true};
    bool _isReady{false};
};
}

#endif // ENGINE_H

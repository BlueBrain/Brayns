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
    virtual ~Engine();

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
       Sets default epsilon to scene bounding box
    */
    void setDefaultEpsilon();

    /**
       Initializes materials for the current scene
       @param colorMap Predefined color map
    */
    void initializeMaterials(
        MaterialsColorMap colorMap = MaterialsColorMap::none);

    /** Mark the scene for building with Brayns::buildScene(). */
    void markRebuildScene(const bool rebuild = true)
    {
        _rebuildScene = rebuild;
    }

    /** @return true if Brayns::buildScene() shall be called. */
    bool rebuildScene() const { return _rebuildScene; }
    /**
     * Callback when a new frame shall be triggered. Currently called by event
     * plugins Deflect and Rockets.
     */
    std::function<void()> triggerRender;

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
    Statistics& getStatistics() { return _statistics; }
    using SnapshotReadyCallback = std::function<void(FrameBufferPtr)>;

    /**
     * Setup render of a snapshot with the given parameters. Calls to render()
     * start updating the framebuffer that will be provided in the ready
     * callback, once the snapshot is ready according to given parameters.
     * Currently determined by the number of accumulation samples.
     *
     * Once the snaphot is done or cancelled, the framebuffer is reset and
     * render() continues normally.
     *
     * If the snapshot creation has been cancelled with cancelSnapshot(), the
     * ready callback will not be called.
     *
     * @param params the snapshot parameter to take
     * @param cb callback when the snapshot is ready and can be obtained from
     *           the given framebuffer
     * @throws std::runtime_error if a previous snapshot creation has not been
     *                            finished yet
     */
    void snapshot(const SnapshotParams& params, SnapshotReadyCallback cb);

    /**
     * Cancel a current pending snapshot. Will reset the framebuffer, so that
     * render() continues normally.
     */
    void cancelSnapshot() { _snapshotCancelled = true; }
    /**
     * @return true if render() calls shall be continued, based on current
     *         accumulation and snapshot settings.
     * @sa RenderingParameters::setMaxAccumFrames
     */
    bool continueRendering() const;

    /** Factory method to create an engine-specific framebuffer. */
    virtual FrameBufferPtr createFrameBuffer(
        const Vector2ui& frameSize, FrameBufferFormat frameBufferFormat,
        bool accumulation) = 0;

    /** Factory method to create an engine-specific camera. */
    virtual CameraPtr createCamera(const CameraType type) = 0;

protected:
    void _render(const RenderInput& renderInput, RenderOutput& renderOutput);
    void _render();

    void _processSnapshot();
    void _writeFrameToFile();

    ParametersManager& _parametersManager;
    ScenePtr _scene;
    CameraPtr _camera;
    MaterialManagerPtr _materialManager;
    RendererType _activeRenderer;
    RendererMap _renderers;
    Vector2i _frameSize;
    FrameBufferPtr _frameBuffer;
    Statistics _statistics;

    Progress _progress;
    bool _keepRunning{true};
    bool _rebuildScene{false};

    int _snapshotSpp{0};
    int _restoreSpp{0};
    SnapshotReadyCallback _cb;
    FrameBufferPtr _snapshotFrameBuffer;
    CameraPtr _snapshotCamera;
    bool _snapshotCancelled{false};
};
}

#endif // ENGINE_H

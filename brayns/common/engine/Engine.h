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

#include <brayns/common/types.h>

namespace brayns
{
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
    virtual std::string name() const = 0;

    /**
       Commits changes to the engine. This include scene and camera
       modifications
    */
    virtual void commit();

    /**
     * Called after the given extension plugin was created to perform
     * engine-specific initialization on a given extension.
     */
    virtual void extensionInit(ExtensionPlugin&){};

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
    /** Gets the parameters manager */
    ParametersManager& getParametersManager() { return _parametersManager; }
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
       @param materialType Predefined sets of colors
             MT_DEFAULT: Random colors
             MT_RANDOM: Random materials including transparency, reflection and
                        light emission
             MT_SHADES_OF_GREY: 255 shades of grey
             MT_GRADIENT: Gradient from red to yellow
             MT_PASTEL_COLORS: Random pastel colors
    */
    void initializeMaterials(MaterialType materialType = MaterialType::none);

    /**
     * Recreates the engine according to new parameters, e.g. datasource has
     * changed or engine type/name. The recreation is delegated to the Brayns
     * instance.
     */
    std::function<void()> recreate;

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

    /**
     * @return the last operation processed by the engine
     */
    const std::string getLastOperation() const { return _lastOperation; }
    /**
     * @return the last normalized progress value (0..1) emitted by the engine
     */
    float getLastProgress() const { return _lastProgress; }
    /**
     * @return true if for "--module deflect" the DeflectPixelOp was
     *         successfully loaded. Only supported for the OSPRay engine.
     */
    virtual bool haveDeflectPixelOp() const { return false; }
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

    size_t _frameNumber;
    float _lastProgress;
    std::string _lastOperation;
};
}

#endif // ENGINE_H

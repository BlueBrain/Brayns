/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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
     * @param parametersManager holds all engine parameters (geometry, rendering, etc)
     */
    explicit Engine( ParametersManager& parametersManager );
    virtual ~Engine() {}

    /**
       @return the name of the engine
    */
    virtual std::string name() const = 0;

    /**
       Commits changes to the engine. This include scene and camera
       modifications
    */
    virtual void commit();

    /** Renders the current scene and populates the frame buffer accordingly */
    virtual void render() = 0;

    /** Executes engine specific pre-render operations */
    virtual void preRender() {}

    /** Executes engine specific post-render operations */
    virtual void postRender() {}

    /** Gets the scene */
    Scene& getScene() { return *_scene; }

    /** Gets the frame buffer */
    FrameBuffer& getFrameBuffer() { return *_frameBuffer; }

    /** Gets the camera */
    Camera& getCamera() { return *_camera; }

    /** Gets the renderer */
    Renderer& getRenderer();

    /** Active renderer */
    void setActiveRenderer( const RendererType renderer );
    RendererType getActiveRenderer() { return _activeRenderer; }

    /**
       Reshapes the current frame buffers
       @param frameSize New size for the buffers

       @todo Must be removed and held by the render method above
    */
    void reshape( const Vector2ui& frameSize );

    /**
       Sets up camera manipulator
    */
    void setupCameraManipulator( const CameraMode mode );

    /**
       Sets default camera according to scene bounding box
    */
    void setDefaultCamera();

    /**
       Sets default epsilon to scene bounding box
    */
    void setDefaultEpsilon();

    /**
       @brief Makes the engine. This means that all attributes, including geometry, material,
       camera, framebuffer, etc, have to be reset according to the engine parameters stored in the
       _parametersManager class member.
    */
    void makeDirty() { _dirty = true; }

    /**
     * @brief isDirty returns the engine state
     * @return True if the engine is dirty and needs to be updated. False otherwise.
     */
    bool isDirty() { return _dirty; }

    /**
       Initializes materials for the current scene
       @param materialType Predefined sets of colors
             MT_DEFAULT: Random colors
             MT_RANDOM: Random materials including transparency, reflection and
                        light emission
             MT_SHADES_OF_GREY: 255 shades of grey
             MT_GRADIENT: Gradient from red to yellow
             MT_PASTEL_COLORS: Random pastel colors
       @param nbMaterials Number of materials to be defined
    */
    void initializeMaterials(
        MaterialType materialType = MT_DEFAULT,
        size_t nbMaterials = NB_MAX_MATERIALS );

protected:

    void _render( const RenderInput& renderInput, RenderOutput& renderOutput );
    void _render();

    ParametersManager& _parametersManager;
    ScenePtr _scene;
    CameraPtr _camera;
    RendererType _activeRenderer;
    RendererMap _renderers;
    Vector2i _frameSize;
    FrameBufferPtr _frameBuffer;
    bool _dirty;

};

}

#endif // ENGINE_H

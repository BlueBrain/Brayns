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

    virtual ~Engine() {}

    /**
       Returns the name of the engine
       @return the name of the engine
    */
    virtual std::string name() const = 0;

    /**
       Commits changes to the engine. This include scene and camera
       modifications
    */
    virtual void commit() = 0;

    /** Renders the current scene and populates the frame buffer accordingly */
    virtual void render() = 0;

    /** Executes engine specific pre-render operations */
    virtual void preRender() = 0;

    /** Executes engine specific post-render operations */
    virtual void postRender() = 0;

    /** Gets the scene */
    ScenePtr getScene() { return _scene; }

    /** Gets the frame buffer */
    FrameBufferPtr getFrameBuffer() { return _frameBuffer; }

    /** Gets the camera */
    CameraPtr getCamera() { return _camera; }

    /** Gets the renderer */
    RendererPtr getRenderer() { return _renderers[_activeRenderer]; }

    /** Sets the active renderer */
    void setActiveRenderer( const std::string renderer );

protected:

    ScenePtr _scene;
    CameraPtr _camera;
    std::string _activeRenderer;
    strings _rendererNames;
    RendererMap _renderers;
    Vector2i _frameSize;
    FrameBufferPtr _frameBuffer;

};

}

#endif // ENGINE_H

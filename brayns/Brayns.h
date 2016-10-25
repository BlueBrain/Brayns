/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef BRAYNS_H
#define BRAYNS_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/common/material/Material.h>

namespace brayns
{

struct RenderInput
{
    Vector2i windowSize;
    Matrix4f modelview;
    Matrix4f projection;

    Vector3f position;
    Vector3f target;
    Vector3f up;
};

struct RenderOutput
{
    uint8_ts colorBuffer;
    floats depthBuffer;
};

/**
    Brayns is a minimalistic library that allows optimized ray-tracing rendering
    of meshes and parametric geometry. Brayns provides an abstraction of the
    underlying rendering engines, making it possible to use the best rendering
    engine depending on the case.

    The scene is initialized according to parameters provided to the Brayns
    constructor. Those parameters are related to the application itself, the
    geometry and the renderer. During the initialization process, Brayns creates
    the scene using loaders located in the braynsPlugins library.

    The underlying rendering engine (OSPRay, Optix, FireRays, etc) is specified
    in the rendering parameters and is invoked by the render method for
    generating the frames.

    Underlying rendering engines support CPU, GPU and heterogenous architectures

    This object exposes the basic API for Brayns
*/
class Brayns
{
public:

    BRAYNS_API Brayns(int argc, const char **argv);
    BRAYNS_API ~Brayns();

    /**
       Renders color and depth buffers of the current scene, according to
       specified parameters
       @param renderInput Rendering parameters such as the position of the
              camera and according model and projection matrices
       @param renderOutput Color and depth buffers
    */
    BRAYNS_API void render(
        const RenderInput& renderInput,
        RenderOutput& renderOutput);

    /**
       Renders color and depth buffers of the current scene, according to
       default parameters. This is typicaly used by an application that does
       not provide any on-screen visualization. In such cases, input and output
       parameters are provided by ZeroEQ events. For instance, a camera event
       defines the origin, target and up vector of the camera, and an ImageJPEG
       event triggers the rendering and gathers the results in a form of a
       base64 encoded JPEG image.
    */
    BRAYNS_API void render();

    /**
       Commits the changes held by scene and camera objects so that
       attributes become available to the underlying rendering engine

       @todo Must be removed (VIZTM-572)
    */
    BRAYNS_API void commit();

    /**
       Gets parameters manager
       @return Parameters manager for the current scene
    */
    BRAYNS_API ParametersManager& getParametersManager();

    /**
       Sets materials for the current scene
       @param materialType Predefined sets of colors
             MT_DEFAULT: Random colors
             MT_RANDOM: Random materials including transparency, reflection and
                        light emission
             MT_SHADES_OF_GREY: 255 shades of grey
             MT_GRADIENT: Gradient from red to yellow
             MT_PASTEL_COLORS: Random pastel colors
       @param nbMaterials Number of materials to be defined

       @todo Must be moved to scene object (VIZTM-572)
    */
    BRAYNS_API void setMaterials(
        MaterialType materialType,
        size_t nbMaterials = NB_MAX_MATERIALS);

    /**
       Reshapes the current frame buffers
       @param frameSize New size for the buffers

       @todo Must be removed and held by the render method above
    */
    BRAYNS_API void reshape( const Vector2ui& frameSize );

    /**
       Gets the current scene
       @return The scene object

       @todo Must be removed (VIZTM-572)
    */
    BRAYNS_API Scene& getScene();

    /**
       Gets the current camera
       @return The camera object

       @todo Must be removed (VIZTM-572)
    */
    BRAYNS_API Camera& getCamera();

    /**
       Gets the current frame buffer
       @return The frame buffer object
    */
    BRAYNS_API FrameBuffer& getFrameBuffer();

    /**
       Gets the keyboard handler
       @return The keyboard handler object
    */
    BRAYNS_API KeyboardHandler& getKeyboardHandler();

private:

    struct Impl;

    std::unique_ptr< Impl > _impl;

};

}
#endif // BRAYNS

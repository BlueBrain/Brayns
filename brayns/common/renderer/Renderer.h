/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <brayns/api.h>
#include <brayns/common/parameters/RenderingParameters.h>

namespace brayns
{

class Renderer
{
public:
    BRAYNS_API Renderer( RenderingParameters& renderingParameters );
    BRAYNS_API virtual ~Renderer( ) {}

    BRAYNS_API virtual void render( FrameBufferPtr frameBuffer ) = 0;

    BRAYNS_API virtual void commit( ) =  0;
    BRAYNS_API void setScene( ScenePtr scene ) { _scene = scene; };
    BRAYNS_API virtual void setCamera( CameraPtr camera ) =  0;

protected:
    RenderingParameters& _renderingParameters;
    ScenePtr _scene;
};

}
#endif // RENDERER_H

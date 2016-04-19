/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef OSPRAYRENDERER_H
#define OSPRAYRENDERER_H

#include <brayns/common/types.h>
#include <brayns/common/renderer/Renderer.h>

#include <ospray.h>

#include "OSPRayCamera.h"

namespace brayns
{

class OSPRayRenderer : public brayns::Renderer
{
public:
    OSPRayRenderer(
        const std::string& name, ParametersManager& parametersMamager );

    void render( FrameBufferPtr frameBuffer ) final;
    void commit() final;

    void setCamera( CameraPtr camera ) final;

    OSPRenderer impl() { return _renderer; }

private:
    OSPRayCamera* _camera;
    OSPRenderer _renderer;
};

}

#endif // OSPRAYRENDERER_H

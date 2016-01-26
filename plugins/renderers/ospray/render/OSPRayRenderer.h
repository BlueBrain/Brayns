/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef OSPRAYRENDERER_H
#define OSPRAYRENDERER_H

#include <brayns/common/types.h>
#include <brayns/common/renderer/Renderer.h>

#include <ospray.h>

namespace brayns
{

class OSPRayRenderer : public brayns::Renderer
{
public:
    OSPRayRenderer( RenderingParameters& renderingParameters );

    void render( FrameBufferPtr frameBuffer ) final;
    void commit() final;

    void setScene( ScenePtr scene ) final;
    void setCamera( CameraPtr camera ) final;

    OSPRenderer impl() { return _renderer; }

private:
    OSPRenderer _renderer;
};

}

#endif // OSPRAYRENDERER_H

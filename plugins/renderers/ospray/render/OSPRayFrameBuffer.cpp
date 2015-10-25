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

#include "OSPRayFrameBuffer.h"

#include <brayns/common/log.h>
#include <ospray/common/OSPCommon.h>

namespace brayns
{

const float DEFAULT_GAMMA = 2.2f;

OSPRayFrameBuffer::OSPRayFrameBuffer(const Vector2i& frameSize, const FrameBufferFormat colorDepth)
    : FrameBuffer(frameSize, colorDepth)
    , _frameBuffer(0)
    , _colorBuffer(0)
    , _depthBuffer(0)
{
    resize(frameSize);
}

OSPRayFrameBuffer::~OSPRayFrameBuffer()
{
    unmap();
    ospFreeFrameBuffer(_frameBuffer);
}

void OSPRayFrameBuffer::resize(const Vector2i& frameSize)
{
    _frameSize = frameSize;

    if( _frameBuffer )
    {
        unmap();
        ospFreeFrameBuffer(_frameBuffer);
    }

    OSPFrameBufferFormat format;
    switch(_frameBufferFormat)
    {
        case FBF_RGBA_I8:
            format = OSP_RGBA_I8;
            break;
        case FBF_RGB_I8:
            format = OSP_RGB_I8;
            break;
        case FBF_RGBA_F32:
            format = OSP_RGBA_F32;
            break;
        default:
            format = OSP_RGBA_NONE;
    }

    ospray::vec2i size(_frameSize.x(), _frameSize.y());
    _frameBuffer = ospNewFrameBuffer( size,
        format, OSP_FB_COLOR|OSP_FB_DEPTH|OSP_FB_ACCUM|OSP_FB_ALPHA );
    ospSet1f(_frameBuffer, "gamma", DEFAULT_GAMMA);
    ospCommit(_frameBuffer);
    clear();
}

void OSPRayFrameBuffer::clear()
{
    ospFrameBufferClear(_frameBuffer, OSP_FB_ACCUM);
}

void OSPRayFrameBuffer::map()
{
    _colorBuffer = (uint8_t *)ospMapFrameBuffer( _frameBuffer, OSP_FB_COLOR );
    _depthBuffer = (float *)ospMapFrameBuffer( _frameBuffer, OSP_FB_DEPTH );
}

void OSPRayFrameBuffer::unmap()
{
    if( _colorBuffer )
    {
        ospUnmapFrameBuffer( _colorBuffer, _frameBuffer );
        _colorBuffer  = 0;
    }

    if( _depthBuffer )
    {
        ospUnmapFrameBuffer( _depthBuffer, _frameBuffer );
        _depthBuffer  = 0;
    }
}

}

/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
            format = OSP_FB_SRGBA; // maybe OSP_FB_RGBA8;
            break;
        case FBF_RGBA_F32:
            format = OSP_FB_RGBA32F;
            break;
        default:
            format = OSP_FB_NONE;
    }

    osp::vec2i size = { _frameSize.x(), _frameSize.y() };
    _frameBuffer = ospNewFrameBuffer( size,
        format, OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM );
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
        _colorBuffer = 0;
    }

    if( _depthBuffer )
    {
        ospUnmapFrameBuffer( _depthBuffer, _frameBuffer );
        _depthBuffer = 0;
    }
}

}

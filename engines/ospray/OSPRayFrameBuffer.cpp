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

#include "OSPRayFrameBuffer.h"

#include <brayns/common/log.h>
#include <brayns/parameters/StreamParameters.h>
#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
OSPRayFrameBuffer::OSPRayFrameBuffer(const Vector2ui& frameSize,
                                     const FrameBufferFormat colorDepth,
                                     const bool accumulation)
    : FrameBuffer(frameSize, colorDepth, accumulation)
    , _frameBuffer(0)
    , _colorBuffer(0)
    , _depthBuffer(0)
{
    resize(frameSize);
}

OSPRayFrameBuffer::~OSPRayFrameBuffer()
{
    auto lock = getScopeLock();

    _unmapUnsafe();
    if (_pixelOp)
        ospRelease(_pixelOp);
    ospRelease(_frameBuffer);
}

void OSPRayFrameBuffer::enableDeflectPixelOp()
{
    _pixelOp = ospNewPixelOp("DeflectPixelOp");
    if (_pixelOp)
    {
        ospCommit(_pixelOp);
        ospSetPixelOp(_frameBuffer, _pixelOp);
    }
}

void OSPRayFrameBuffer::resize(const Vector2ui& frameSize)
{
    if (frameSize.product() == 0)
        throw std::runtime_error("Invalid size for framebuffer resize");

    if (_frameBuffer && getSize() == frameSize)
        return;

    _frameSize = frameSize;

    _recreate();
}

void OSPRayFrameBuffer::setStreamingParams(const StreamParameters& params,
                                           const bool stereo)
{
    if (_pixelOp)
    {
        ospSetString(_pixelOp, "id", params.getId().c_str());
        ospSetString(_pixelOp, "hostname", params.getHostname().c_str());
        ospSet1i(_pixelOp, "port", params.getPort());
        ospSet1i(_pixelOp, "enabled", params.getEnabled());
        ospSet1i(_pixelOp, "compression", params.getCompression());
        ospSet1i(_pixelOp, "quality", params.getQuality());
        ospSet1i(_pixelOp, "stereo", stereo);
        ospCommit(_pixelOp);
    }
}

void OSPRayFrameBuffer::_recreate()
{
    auto lock = getScopeLock();

    if (_frameBuffer)
    {
        _unmapUnsafe();
        ospRelease(_frameBuffer);
    }

    OSPFrameBufferFormat format;
    switch (_frameBufferFormat)
    {
    case FrameBufferFormat::rgba_i8:
        format = OSP_FB_RGBA8;
        break;
    case FrameBufferFormat::rgb_f32:
        format = OSP_FB_RGBA32F;
        break;
    default:
        format = OSP_FB_NONE;
    }

    const osp::vec2i size = {int(_frameSize.x()), int(_frameSize.y())};

    size_t attributes = OSP_FB_COLOR | OSP_FB_DEPTH;
    if (_accumulation)
        attributes |= OSP_FB_ACCUM | OSP_FB_VARIANCE;

    _frameBuffer = ospNewFrameBuffer(size, format, attributes);
    if (_pixelOp)
        ospSetPixelOp(_frameBuffer, _pixelOp);
    ospCommit(_frameBuffer);
    clear();
}

void OSPRayFrameBuffer::clear()
{
    FrameBuffer::clear();
    size_t attributes = OSP_FB_COLOR | OSP_FB_DEPTH;
    if (_accumulation)
        attributes |= OSP_FB_ACCUM | OSP_FB_VARIANCE;
    ospFrameBufferClear(_frameBuffer, attributes);
}

void OSPRayFrameBuffer::map()
{
    _mapMutex.lock();
    _mapUnsafe();
}

void OSPRayFrameBuffer::_mapUnsafe()
{
    if (_frameBufferFormat == FrameBufferFormat::none)
        return;

    _colorBuffer = (uint8_t*)ospMapFrameBuffer(_frameBuffer, OSP_FB_COLOR);
    _depthBuffer = (float*)ospMapFrameBuffer(_frameBuffer, OSP_FB_DEPTH);
}

void OSPRayFrameBuffer::unmap()
{
    _unmapUnsafe();
    _mapMutex.unlock();
}

void OSPRayFrameBuffer::_unmapUnsafe()
{
    if (_frameBufferFormat == FrameBufferFormat::none)
        return;

    if (_colorBuffer)
    {
        ospUnmapFrameBuffer(_colorBuffer, _frameBuffer);
        _colorBuffer = 0;
    }

    if (_depthBuffer)
    {
        ospUnmapFrameBuffer(_depthBuffer, _frameBuffer);
        _depthBuffer = 0;
    }
}

void OSPRayFrameBuffer::setAccumulation(const bool accumulation)
{
    if (_accumulation != accumulation)
    {
        FrameBuffer::setAccumulation(accumulation);
        _recreate();
    }
}
}

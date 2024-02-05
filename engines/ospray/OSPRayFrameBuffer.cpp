/* Copyright 2015-2024 Blue Brain Project/EPFL
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
#include "utils.h"

#include <brayns/common/Log.h>
#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
namespace
{
OSPFrameBufferFormat toOSPFrameBufferFormat(const PixelFormat frameBufferFormat)
{
    switch (frameBufferFormat)
    {
    case PixelFormat::RGBA_I8:
        return OSP_FB_RGBA8;
    case PixelFormat::RGB_F32:
        return OSP_FB_RGBA32F;
    default:
        return OSP_FB_NONE;
    }
}
} // namespace
OSPRayFrameBuffer::OSPRayFrameBuffer(const std::string& name,
                                     const Vector2ui& frameSize,
                                     const PixelFormat frameBufferFormat)
    : FrameBuffer(name, frameSize, frameBufferFormat)
{
    resize(frameSize);
}

OSPRayFrameBuffer::~OSPRayFrameBuffer()
{
    auto lock = getScopeLock();

    _unmapUnsafe();
    ospRelease(_pixelOp);
    ospRelease(_frameBuffer);
    ospRelease(_subsamplingFrameBuffer);
}

void OSPRayFrameBuffer::resize(const Vector2ui& frameSize)
{
    if (glm::compMul(frameSize) == 0)
        throw std::runtime_error("Invalid size for framebuffer resize");

    if (_frameBuffer && _frameSize == frameSize)
        return;

    _frameSize = frameSize;

    _recreate();
}

void OSPRayFrameBuffer::_recreate()
{
    auto lock = getScopeLock();

    _unmapUnsafe();
    ospRelease(_frameBuffer);

    const osp::vec2i size = {int(_frameSize.x), int(_frameSize.y)};

    size_t attributes = OSP_FB_COLOR | OSP_FB_DEPTH;
    if (_accumulation)
        attributes |= OSP_FB_ACCUM | OSP_FB_VARIANCE;

    _frameBuffer =
        ospNewFrameBuffer(size, toOSPFrameBufferFormat(_frameBufferFormat),
                          attributes);
    if (_pixelOp)
        ospSetPixelOp(_frameBuffer, _pixelOp);
    osphelper::set(_frameBuffer, "name", getName());
    ospCommit(_frameBuffer);

    _recreateSubsamplingBuffer();

    clear();
}

void OSPRayFrameBuffer::_recreateSubsamplingBuffer()
{
    ospRelease(_subsamplingFrameBuffer);
    _subsamplingFrameBuffer = nullptr;
    const auto subsamplingSize = _subsamplingSize();
    if (_frameSize != subsamplingSize)
    {
        _subsamplingFrameBuffer =
            ospNewFrameBuffer({int(subsamplingSize.x), int(subsamplingSize.y)},
                              toOSPFrameBufferFormat(_frameBufferFormat),
                              OSP_FB_COLOR | OSP_FB_DEPTH);
        if (_pixelOp)
            ospSetPixelOp(_subsamplingFrameBuffer, _pixelOp);
        ospCommit(_subsamplingFrameBuffer);
    }
}

void OSPRayFrameBuffer::clear()
{
    FrameBuffer::clear();
    size_t attributes = OSP_FB_COLOR | OSP_FB_DEPTH;
    if (_subsamplingFrameBuffer)
        ospFrameBufferClear(_subsamplingFrameBuffer, attributes);
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
    if (_frameBufferFormat == PixelFormat::NONE)
        return;

    _colorBuffer = (uint8_t*)ospMapFrameBuffer(_currentFB(), OSP_FB_COLOR);
    _depthBuffer = (float*)ospMapFrameBuffer(_currentFB(), OSP_FB_DEPTH);
}

void OSPRayFrameBuffer::unmap()
{
    _unmapUnsafe();
    _mapMutex.unlock();
}

void OSPRayFrameBuffer::_unmapUnsafe()
{
    if (_frameBufferFormat == PixelFormat::NONE)
        return;

    if (_colorBuffer)
    {
        ospUnmapFrameBuffer(_colorBuffer, _currentFB());
        _colorBuffer = nullptr;
    }

    if (_depthBuffer)
    {
        ospUnmapFrameBuffer(_depthBuffer, _currentFB());
        _depthBuffer = nullptr;
    }
}

bool OSPRayFrameBuffer::_useSubsampling() const
{
    return _subsamplingFrameBuffer && numAccumFrames() <= 1;
}

OSPFrameBuffer OSPRayFrameBuffer::_currentFB() const
{
    return _useSubsampling() ? _subsamplingFrameBuffer : _frameBuffer;
}

Vector2ui OSPRayFrameBuffer::_subsamplingSize() const
{
    return _frameSize / _subsamplingFactor;
}

void OSPRayFrameBuffer::setAccumulation(const bool accumulation)
{
    if (_accumulation != accumulation)
    {
        FrameBuffer::setAccumulation(accumulation);
        _recreate();
    }
}

void OSPRayFrameBuffer::setFormat(PixelFormat frameBufferFormat)
{
    if (_frameBufferFormat != frameBufferFormat)
    {
        FrameBuffer::setFormat(frameBufferFormat);
        _recreate();
    }
}

void OSPRayFrameBuffer::setSubsampling(size_t factor)
{
    factor = std::max(1ul, factor);
    if (_subsamplingFactor == factor)
        return;

    _subsamplingFactor = factor;
    auto lock = getScopeLock();
    _unmapUnsafe();
    _recreateSubsamplingBuffer();
}

void OSPRayFrameBuffer::createPixelOp(const std::string& name)
{
    if (_pixelOp)
        return;

    _pixelOp = ospNewPixelOp(name.c_str());
    if (_pixelOp)
    {
        ospSetPixelOp(_frameBuffer, _pixelOp);
        if (_subsamplingFrameBuffer)
            ospSetPixelOp(_subsamplingFrameBuffer, _pixelOp);
    }
}

void OSPRayFrameBuffer::updatePixelOp(const PropertyMap& properties)
{
    if (_pixelOp)
    {
        toOSPRayProperties(properties, _pixelOp);
        ospCommit(_pixelOp);
    }
}
} // namespace brayns

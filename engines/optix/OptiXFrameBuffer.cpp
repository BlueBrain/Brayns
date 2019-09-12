/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "OptiXFrameBuffer.h"
#include "OptiXContext.h"

#include <brayns/common/log.h>

#include <optixu/optixu_math_namespace.h>

namespace brayns
{
OptiXFrameBuffer::OptiXFrameBuffer(const std::string& name,
                                   const Vector2ui& frameSize,
                                   FrameBufferFormat frameBufferFormat)
    : FrameBuffer(name, frameSize, frameBufferFormat)
{
    resize(frameSize);
}

OptiXFrameBuffer::~OptiXFrameBuffer()
{
    auto lock = getScopeLock();
    _unmapUnsafe();
    destroy();
}

void OptiXFrameBuffer::destroy()
{
    if (_frameBuffer)
        _frameBuffer->destroy();

    if (_accumBuffer)
        _accumBuffer->destroy();
}

void OptiXFrameBuffer::resize(const Vector2ui& frameSize)
{
    if (glm::compMul(frameSize) == 0)
        throw std::runtime_error("Invalid size for framebuffer resize");

    if (_frameBuffer && getSize() == frameSize)
        return;

    _frameSize = frameSize;

    _recreate();
}

void OptiXFrameBuffer::_recreate()
{
    BRAYNS_DEBUG << "Creating frame buffer..." << std::endl;
    auto lock = getScopeLock();

    if (_frameBuffer)
    {
        _unmapUnsafe();
        destroy();
    }

    RTformat format;
    switch (_frameBufferFormat)
    {
    case FrameBufferFormat::rgb_i8:
        format = RT_FORMAT_UNSIGNED_BYTE3;
        break;
    case FrameBufferFormat::rgba_i8:
    case FrameBufferFormat::bgra_i8:
        format = RT_FORMAT_UNSIGNED_BYTE4;
        break;
    case FrameBufferFormat::rgb_f32:
        format = RT_FORMAT_FLOAT4;
        break;
    default:
        format = RT_FORMAT_UNKNOWN;
    }

    auto context = OptiXContext::get().getOptixContext();
    _frameBuffer = context->createBuffer(RT_BUFFER_OUTPUT, format, _frameSize.x,
                                         _frameSize.y);
    if (_accumulation)
        _accumBuffer =
            context->createBuffer(RT_BUFFER_INPUT_OUTPUT | RT_BUFFER_GPU_LOCAL,
                                  RT_FORMAT_FLOAT4, _frameSize.x, _frameSize.y);
    else
        _accumBuffer =
            context->createBuffer(RT_BUFFER_INPUT_OUTPUT | RT_BUFFER_GPU_LOCAL,
                                  RT_FORMAT_FLOAT4, 1, 1);
    clear();
    BRAYNS_DEBUG << "Frame buffer created" << std::endl;
}

void OptiXFrameBuffer::map()
{
    _mapMutex.lock();
    _mapUnsafe();
}

void OptiXFrameBuffer::_mapUnsafe()
{
    rtBufferMap(_frameBuffer->get(), &_imageData);

    auto context = OptiXContext::get().getOptixContext();
    const auto frame = _accumulation ? static_cast<size_t>(_accumFrames) : 0;

    context["frame"]->setUint(frame);
    context["output_buffer"]->set(_frameBuffer);
    context["accum_buffer"]->set(_accumBuffer);

    switch (_frameBufferFormat)
    {
    case FrameBufferFormat::rgba_i8:
        _colorBuffer = (uint8_t*)(_imageData);
        break;
    case FrameBufferFormat::rgb_f32:
        _depthBuffer = (float*)_imageData;
        break;
    default:
        BRAYNS_ERROR << "Unsupported format" << std::endl;
    }
}

void OptiXFrameBuffer::unmap()
{
    _unmapUnsafe();
    _mapMutex.unlock();
}

void OptiXFrameBuffer::_unmapUnsafe()
{
    if (_frameBufferFormat == FrameBufferFormat::none)
        return;

    rtBufferUnmap(_frameBuffer->get());
    _colorBuffer = nullptr;
    _depthBuffer = nullptr;
}

void OptiXFrameBuffer::setAccumulation(const bool accumulation)
{
    if (_accumulation != accumulation)
    {
        FrameBuffer::setAccumulation(accumulation);
        _recreate();
    }
}

} // namespace brayns

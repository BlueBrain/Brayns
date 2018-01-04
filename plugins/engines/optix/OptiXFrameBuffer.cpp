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

#include "OptiXFrameBuffer.h"

#include <brayns/common/log.h>

#include <optixu/optixu_math_namespace.h>

namespace
{
const std::string CUDA_OUTPUT_BUFFER = "output_buffer";
const std::string CUDA_ACCUMULATION_BUFFER = "accum_buffer";
const std::string CUDA_FRAME_NUMBER = "frame_number";
}

namespace brayns
{
OptiXFrameBuffer::OptiXFrameBuffer(const Vector2ui& frameSize,
                                   const FrameBufferFormat colorDepth,
                                   const bool accumulation,
                                   optix::Context& context)
    : FrameBuffer(frameSize, colorDepth, accumulation)
    , _frameBuffer(nullptr)
    , _accumBuffer(nullptr)
    , _context(context)
    , _colorBuffer(nullptr)
    , _depthBuffer(nullptr)
    , _accumulationFrameNumber(1u)
    , _imageData(nullptr)
{
    resize(frameSize);
}

OptiXFrameBuffer::~OptiXFrameBuffer()
{
    _cleanup();
}

void OptiXFrameBuffer::_cleanup()
{
    unmap();
    if (_frameBuffer)
        _frameBuffer->destroy();
    _frameBuffer = nullptr;
    if (_accumBuffer)
        _accumBuffer->destroy();
    _accumBuffer = nullptr;
}

void OptiXFrameBuffer::resize(const Vector2ui& frameSize)
{
    _frameSize = frameSize;
    _cleanup();

    RTformat format;
    switch (_frameBufferFormat)
    {
    case FrameBufferFormat::rgba_i8:
        format = RT_FORMAT_UNSIGNED_BYTE4;
        break;
    case FrameBufferFormat::rgb_f32:
        format = RT_FORMAT_FLOAT4;
        break;
    default:
        format = RT_FORMAT_UNKNOWN;
    }

    _frameBuffer = _context->createBuffer(RT_BUFFER_OUTPUT, format,
                                          _frameSize.x(), _frameSize.y());
    _context[CUDA_OUTPUT_BUFFER]->set(_frameBuffer);

    _accumBuffer =
        _context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4,
                               _frameSize.x(), _frameSize.y());

    _context[CUDA_ACCUMULATION_BUFFER]->set(_accumBuffer);

    clear();
}

void OptiXFrameBuffer::clear()
{
    _accumulationFrameNumber = 1u;
}

void OptiXFrameBuffer::map()
{
    // Now unmap the buffer
    if (!_frameBuffer)
        return;

    rtBufferMap(_frameBuffer->get(), &_imageData);

    if (_accumulation)
        _context[CUDA_FRAME_NUMBER]->setUint(_accumulationFrameNumber++);
    else
        _context[CUDA_FRAME_NUMBER]->setUint(1u);

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
    // Now unmap the buffer
    if (!_frameBuffer)
        return;

    rtBufferUnmap(_frameBuffer->get());
    _colorBuffer = nullptr;
    _depthBuffer = nullptr;
}
}

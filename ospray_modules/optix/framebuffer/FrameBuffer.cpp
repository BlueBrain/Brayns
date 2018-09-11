/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include "FrameBuffer.h"

#include "../Context.h"

namespace bbp
{
namespace optix
{
FrameBuffer::FrameBuffer(const ospray::vec2i& size_,
                         ColorBufferFormat colorBufferFormat_,
                         bool hasDepthBuffer_, bool hasAccumBuffer_,
                         bool hasVarianceBuffer_)
    : ospray::FrameBuffer(size_, colorBufferFormat_, hasDepthBuffer_,
                          hasAccumBuffer_, hasVarianceBuffer_)
    , _context(Context::get().getOptixContext())
    , _accumulation(hasAccumBuffer_)
{
    RTformat format;
    switch (colorBufferFormat)
    {
    case OSP_FB_RGBA8:
    case OSP_FB_SRGBA:
        format = RT_FORMAT_UNSIGNED_BYTE4;
        break;
    case OSP_FB_RGBA32F:
        format = RT_FORMAT_FLOAT4;
        break;
    case OSP_FB_NONE:
        return;
    default:
        format = RT_FORMAT_UNKNOWN;
    }

    _frameBuffer =
        _context->createBuffer(RT_BUFFER_OUTPUT, format, size.x, size.y);
    _context["output_buffer"]->set(_frameBuffer);

    _accumBuffer = _context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
                                          RT_FORMAT_FLOAT4, size.x, size.y);

    _context["accum_buffer"]->set(_accumBuffer);
}

FrameBuffer::~FrameBuffer()
{
    if (_frameBuffer)
        _frameBuffer->destroy();
    if (_accumBuffer)
        _accumBuffer->destroy();
}

std::string FrameBuffer::toString() const
{
    return "ospray::FrameBuffer";
}

void FrameBuffer::clear(const ospray::uint32 fbChannelFlags)
{
    frameID = -1; // we increment at the start of the frame

    if (fbChannelFlags & OSP_FB_ACCUM)
        _accumulationFrame = 1;
}

void FrameBuffer::setTile(ospray::Tile& tile)
{
    if (pixelOp)
        pixelOp->preAccum(tile);
    if (pixelOp)
        pixelOp->postAccum(tile);
}

ospray::int32 FrameBuffer::accumID(const ospray::vec2i&)
{
    NOTIMPLEMENTED
}

float FrameBuffer::tileError(const ospray::vec2i&)
{
    NOTIMPLEMENTED
}

#ifdef NEW_OSPRAY
void FrameBuffer::beginFrame(const float /*errorThreshold*/)
#else
void FrameBuffer::beginFrame()
#endif
{
    frameID++;
    if (pixelOp)
        pixelOp->beginFrame();
}

float FrameBuffer::endFrame(const float /*errorThreshold*/)
{
    if (pixelOp)
        pixelOp->endFrame();
    return ospcommon::inf; // keep rendering by lying of still having errors
}

const void* FrameBuffer::mapDepthBuffer()
{
    this->refInc();
    if (colorBuffer)
        depthBuffer = (float*)colorBuffer;
    return (const void*)depthBuffer;
}

const void* FrameBuffer::mapColorBuffer()
{
    this->refInc();
    rtBufferMap(_frameBuffer->get(), &colorBuffer);
    depthBuffer = (float*)colorBuffer;
    if (_accumulation)
        _context["frame_number"]->setUint(_accumulationFrame++);
    else
        _context["frame_number"]->setUint(1u);
    return (const void*)colorBuffer;
}

void FrameBuffer::unmap(const void* mappedMem)
{
    if (!(mappedMem == colorBuffer || mappedMem == depthBuffer))
    {
        throw std::runtime_error(
            "ERROR: unmapping a pointer not created by "
            "OSPRay!");
    }
    this->refDec();
    rtBufferUnmap(_frameBuffer->get());
    colorBuffer = nullptr;
    depthBuffer = nullptr;
}
}
}

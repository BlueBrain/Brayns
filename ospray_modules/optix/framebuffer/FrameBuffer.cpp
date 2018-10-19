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
                         const ColorBufferFormat colorBufferFormat_,
                         const ospray::uint32 channels)
    : ospray::FrameBuffer(size_, colorBufferFormat_, channels)
    , _context(Context::get().getOptixContext())
    , _accumulation(channels & OSP_FB_ACCUM)
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

void FrameBuffer::beginFrame()
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

const void* FrameBuffer::mapBuffer(const OSPFrameBufferChannel channel)
{
    if (channel == OSP_FB_COLOR)
    {
        if (!colorBuffer)
            _mapColorBuffer();
        return colorBuffer;
    }
    return nullptr;
}

void FrameBuffer::unmap(const void* mappedMem)
{
    if (mappedMem != colorBuffer)
        throw std::runtime_error(
            "ERROR: unmapping a pointer not created by Optix!");

    rtBufferUnmap(_frameBuffer->get());
    colorBuffer = nullptr;
}

void FrameBuffer::_mapColorBuffer()
{
    rtBufferMap(_frameBuffer->get(), &colorBuffer);

    if (_accumulation)
        _context["frame_number"]->setUint(_accumulationFrame++);
    else
        _context["frame_number"]->setUint(1u);
}

}
}

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

#pragma once

#include <ospray/SDK/fb/FrameBuffer.h>
#include <ospray/SDK/fb/TileError.h>

#include <optixu/optixpp_namespace.h>

namespace bbp
{
namespace optix
{
struct FrameBuffer : public ospray::FrameBuffer
{
    FrameBuffer(const ospray::vec2i& size, ColorBufferFormat colorBufferFormat,
                const ospray::uint32 channels);
    ~FrameBuffer();

    //! \brief common function to help printf-debugging
    /*! \detailed Every derived class should override this! */
    std::string toString() const override;

    void setTile(ospray::Tile& tile) override;
    ospray::int32 accumID(const ospray::vec2i& tile) override;
    float tileError(const ospray::vec2i& tile) override;
    void beginFrame() override;
    float endFrame(const float errorThreshold) override;

    const void* mapBuffer(OSPFrameBufferChannel channel) override;
    void unmap(const void* mappedMem) override;
    void clear(const ospray::uint32 fbChannelFlags) override;

private:
    void* colorBuffer{nullptr};
    //void* depthBuffer{nullptr};

    ::optix::Buffer _frameBuffer;
    ::optix::Buffer _accumBuffer;
    ::optix::Context _context;
    uint16_t _accumulationFrame{0};
    bool _accumulation{true};

    void _mapColorBuffer();
};
}
}

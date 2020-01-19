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

#pragma once

#include <brayns/engineapi/FrameBuffer.h>
#include <optixu/optixpp_namespace.h>

#include <mutex>

namespace brayns
{
/**
   OptiX specific frame buffer

   This object is the OptiX specific implementation of a frame buffer
*/
class OptiXFrameBuffer : public FrameBuffer
{
public:
    OptiXFrameBuffer(const std::string& name, const Vector2ui& size,
                     FrameBufferFormat frameBufferFormat);
    ~OptiXFrameBuffer();

    void resize(const Vector2ui& size) final;
    void map() final;
    void unmap() final;
    void setAccumulation(const bool accumulation) final;

    std::unique_lock<std::mutex> getScopeLock()
    {
        return std::unique_lock<std::mutex>(_mapMutex);
    }
    const uint8_t* getColorBuffer() const final { return _colorBuffer; }
    const float* getDepthBuffer() const final { return 0; }

private:
    void destroy();
    void _recreate();
    void _mapUnsafe();
    void _unmapUnsafe();

    optix::Buffer _frameBuffer{nullptr};
    optix::Buffer _accumBuffer{nullptr};
    uint8_t* _colorBuffer{nullptr};
    float* _depthBuffer{nullptr};
    void* _imageData{nullptr};

    // protect map/unmap
    std::mutex _mapMutex;
};
} // namespace brayns

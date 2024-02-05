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

#pragma once

#include <brayns/engine/FrameBuffer.h>
#include <ospray.h>

#include <mutex>

namespace brayns
{
class OSPRayFrameBuffer : public FrameBuffer
{
public:
    OSPRayFrameBuffer(const std::string& name, const Vector2ui& frameSize,
                      const PixelFormat frameBufferFormat);
    ~OSPRayFrameBuffer();

    void clear() final;
    void resize(const Vector2ui& frameSize) final;
    void map() final;
    void unmap() final;
    void setAccumulation(const bool accumulation) final;
    void setFormat(PixelFormat frameBufferFormat) final;
    void setSubsampling(const size_t) final;
    Vector2ui getSize() const final
    {
        return _useSubsampling() ? _subsamplingSize() : _frameSize;
    }
    std::unique_lock<std::mutex> getScopeLock()
    {
        return std::unique_lock<std::mutex>(_mapMutex);
    }
    const uint8_t* getColorBuffer() const final { return _colorBuffer; }
    const float* getDepthBuffer() const final { return _depthBuffer; }
    OSPFrameBuffer impl() { return _currentFB(); }
    void createPixelOp(const std::string& name) final;
    void updatePixelOp(const PropertyMap& properties) final;

private:
    void _recreate();
    void _recreateSubsamplingBuffer();
    void _unmapUnsafe();
    void _mapUnsafe();
    bool _useSubsampling() const;
    OSPFrameBuffer _currentFB() const;
    Vector2ui _subsamplingSize() const;

    OSPFrameBuffer _frameBuffer{nullptr};
    OSPFrameBuffer _subsamplingFrameBuffer{nullptr};
    uint8_t* _colorBuffer{nullptr};
    float* _depthBuffer{nullptr};
    OSPPixelOp _pixelOp{nullptr};
    size_t _subsamplingFactor{1};

    // protect map/unmap vs ospRenderFrame
    std::mutex _mapMutex;
};
} // namespace brayns

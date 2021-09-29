/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#ifndef PBRTFRAMEBUFFER_H
#define PBRTFRAMEBUFFER_H

#include <brayns/engine/FrameBuffer.h>
#include <pbrt/core/pbrt.h>

#include <mutex>

namespace brayns
{
class PBRTFrameBuffer : public FrameBuffer
{
public:
    PBRTFrameBuffer(const std::string& name, const Vector2ui& frameSize,
                    const FrameBufferFormat frameBufferFormat);
    ~PBRTFrameBuffer() = default;

    void resize(const Vector2ui& frameSize) final;

    void map() final;

    void unmap() final;

    void fillColorBuffer(const std::vector<pbrt::Float>& src);

    const uint8_t* getColorBuffer() const final { return _colorBuffer.data(); }

    const float* getDepthBuffer() const final { return nullptr; }

    std::unique_lock<std::mutex> getScopeLock()
    {
        return std::unique_lock<std::mutex>(_mapMutex);
    }

    void setBackgroundColor(const Vector3d& color)
    {
        _backgroundColor[0] = static_cast<uint8_t>(color.x * 255.0);
        _backgroundColor[1] = static_cast<uint8_t>(color.y * 255.0);
        _backgroundColor[2] = static_cast<uint8_t>(color.z * 255.0);
        _backgroundColor[3] = 0;
    }

private:
    uint8_ts _colorBuffer;
    std::mutex _mapMutex;

    std::array<uint8_t, 4> _backgroundColor{0, 0, 0, 0};
};
} // namespace brayns

#endif

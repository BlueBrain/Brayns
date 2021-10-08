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

#include "PBRTFrameBuffer.h"
#include <pbrt/core/imageio.h>

// From PBRT src/core/film.cpp
#define BRAYNS_PBRT_TO_BYTE(v) \
    static_cast<uint8_t>(      \
        pbrt::Clamp(255.f * pbrt::GammaCorrect(v) + 0.5f, 0.f, 255.f))

namespace brayns
{
PBRTFrameBuffer::PBRTFrameBuffer(const std::string& name,
                                 const Vector2ui& frameSize,
                                 const FrameBufferFormat frameBufferFormat)
    : FrameBuffer(name, frameSize, frameBufferFormat)
{
    setAccumulation(false);
    _colorBuffer.resize(frameSize.x * frameSize.y * getColorDepth(), 0u);
}

void PBRTFrameBuffer::fillColorBuffer(const std::vector<pbrt::Float>& src)
{
    const size_t colorDepth = getColorDepth();
    if (colorDepth == 0)
        throw std::runtime_error(
            "PBRTFrameBuffer: 0 depth color is not supported");

    if ((colorDepth == 3 && _colorBuffer.size() % 3 != 0) ||
        (colorDepth == 4 && _colorBuffer.size() % 4 != 0))
        throw std::runtime_error(
            "PBRTFrameBuffer: Color buffer does not match color depth");

    for (size_t i = 0; i < _colorBuffer.size(); ++i)
        _colorBuffer[i] = _backgroundColor[i % colorDepth];

        // std::fill(_colorBuffer.begin(), _colorBuffer.end(), 255u);

#pragma omp parallel for
    for (uint32_t x = 0; x < _frameSize.x; ++x)
    {
        for (uint32_t y = 0; y < _frameSize.y; ++y)
        {
            auto r = BRAYNS_PBRT_TO_BYTE(src[3 * (x * _frameSize.y + y) + 0]);
            auto g = BRAYNS_PBRT_TO_BYTE(src[3 * (x * _frameSize.y + y) + 1]);
            auto b = BRAYNS_PBRT_TO_BYTE(src[3 * (x * _frameSize.y + y) + 2]);
            if (r > 0 && g > 0 && b > 0)
            {
                uint8_t* dst = &_colorBuffer[x * (_frameSize.y * colorDepth) +
                                             (y * colorDepth)];
                dst[0] = r;
                dst[1] = g;
                dst[2] = b;
                if (colorDepth == 4)
                    dst[3] = 255;
            }
        }
    }
}

void PBRTFrameBuffer::resize(const Vector2ui& frameSize)
{
    if (getSize() == frameSize)
        return;

    _frameSize = frameSize;
    _colorBuffer.resize(frameSize.x * frameSize.y * getColorDepth(), 0u);
}

void PBRTFrameBuffer::map()
{
    _mapMutex.lock();
}

void PBRTFrameBuffer::unmap()
{
    _mapMutex.unlock();
}
} // namespace brayns

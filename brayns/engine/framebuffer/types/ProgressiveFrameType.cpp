/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ProgressiveFrameType.h"

namespace
{
class FramebufferSelector
{
public:
    template<typename FramebufferType>
    static FramebufferType &select(FramebufferType &low, FramebufferType &high)
    {
        if (low.getAccumulationFrameCount() == 0)
        {
            return low;
        }

        return high;
    }
};
}

namespace brayns
{
ProgressiveFrameType::ProgressiveFrameType(uint32_t scale)
    : _scale(scale)
{
    setFrameSize(_highRes.getFrameSize());
}

bool ProgressiveFrameType::commit()
{
    _lowRes.commit();
    return _highRes.commit();
}

void ProgressiveFrameType::setFrameSize(const Vector2ui &frameSize)
{
    auto lowResolution = frameSize / _scale;
    _lowRes.setFrameSize(lowResolution);
    _highRes.setFrameSize(frameSize);
}

const Vector2ui &ProgressiveFrameType::getFrameSize() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).getFrameSize();
}

float ProgressiveFrameType::getAspectRatio() const noexcept
{
    return _highRes.getAspectRatio();
}

void ProgressiveFrameType::setAccumulation(const bool accumulation) noexcept
{
    (void)accumulation;
}

bool ProgressiveFrameType::isAccumulating() const noexcept
{
    return true;
}

void ProgressiveFrameType::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _lowRes.setFormat(frameBufferFormat);
    _highRes.setFormat(frameBufferFormat);
}

PixelFormat ProgressiveFrameType::getFormat() const noexcept
{
    return _highRes.getFormat();
}

void ProgressiveFrameType::clear() noexcept
{
    _lowRes.clear();
    _highRes.clear();
}

void ProgressiveFrameType::incrementAccumFrames() noexcept
{
    FramebufferSelector::select(_lowRes, _highRes).incrementAccumFrames();
}

size_t ProgressiveFrameType::getAccumulationFrameCount() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).getAccumulationFrameCount();
}

bool ProgressiveFrameType::hasNewAccumulationFrame() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).hasNewAccumulationFrame();
}

void ProgressiveFrameType::resetNewAccumulationFrame() noexcept
{
    FramebufferSelector::select(_lowRes, _highRes).resetNewAccumulationFrame();
}

Image ProgressiveFrameType::getImage()
{
    return FramebufferSelector::select(_lowRes, _highRes).getImage();
}

const ospray::cpp::FrameBuffer &ProgressiveFrameType::getHandle() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).getHandle();
}
}

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

#include "ProgressiveFramebuffer.h"

namespace
{
class FramebufferSelector
{
public:
    template<typename FramebufferType>
    static FramebufferType &select(FramebufferType &low, FramebufferType &high)
    {
        if (low.numAccumFrames() == 0)
        {
            return low;
        }

        return high;
    }
};
}

namespace brayns
{
ProgressiveFramebuffer::ProgressiveFramebuffer(uint32_t scale)
    : _scale(scale)
{
    setFrameSize(_highRes.getFrameSize());
}

void ProgressiveFramebuffer::map()
{
    FramebufferSelector::select(_lowRes, _highRes).map();
}

void ProgressiveFramebuffer::unmap()
{
    FramebufferSelector::select(_lowRes, _highRes).unmap();
}

const uint8_t *ProgressiveFramebuffer::getColorBuffer() const
{
    FramebufferSelector::select(_lowRes, _highRes).getColorBuffer();
}

bool ProgressiveFramebuffer::commit()
{
    _lowRes.commit();
    _highRes.commit();
}

void ProgressiveFramebuffer::setFrameSize(const Vector2ui &frameSize)
{
    auto lowResolution = frameSize / _scale;
    _lowRes.setFrameSize(lowResolution);
    _highRes.setFrameSize(frameSize);
}

const Vector2ui &ProgressiveFramebuffer::getFrameSize() const noexcept
{
    FramebufferSelector::select(_lowRes, _highRes).getFrameSize();
}

float ProgressiveFramebuffer::getAspectRatio() const noexcept
{
    return _highRes.getAspectRatio();
}

void ProgressiveFramebuffer::setAccumulation(const bool accumulation) noexcept
{
    (void)accumulation;
}

bool ProgressiveFramebuffer::isAccumulating() const noexcept
{
    return true;
}

void ProgressiveFramebuffer::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _lowRes.setFormat(frameBufferFormat);
    _highRes.setFormat(frameBufferFormat);
}

PixelFormat ProgressiveFramebuffer::getFrameBufferFormat() const noexcept
{
    return _highRes.getFrameBufferFormat();
}

void ProgressiveFramebuffer::clear() noexcept
{
    _lowRes.clear();
    _highRes.clear();
}

void ProgressiveFramebuffer::incrementAccumFrames() noexcept
{
    FramebufferSelector::select(_lowRes, _highRes).incrementAccumFrames();
}

int32_t ProgressiveFramebuffer::numAccumFrames() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).numAccumFrames();
}

bool ProgressiveFramebuffer::hasNewAccumulationFrame() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).hasNewAccumulationFrame();
}

void ProgressiveFramebuffer::resetNewAccumulationFrame() noexcept
{
    FramebufferSelector::select(_lowRes, _highRes).resetNewAccumulationFrame();
}

Image ProgressiveFramebuffer::getImage()
{
    return FramebufferSelector::select(_lowRes, _highRes).getImage();
}

const ospray::cpp::FrameBuffer &ProgressiveFramebuffer::getHandle() const noexcept
{
    return FramebufferSelector::select(_lowRes, _highRes).getHandle();
}
}

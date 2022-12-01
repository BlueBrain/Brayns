/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "ProgressiveFrameHandler.h"

namespace brayns
{
ProgressiveFrameHandler::ProgressiveFrameHandler(uint32_t scale)
    : _scale(scale)
{
    if (_scale == 0)
    {
        throw std::invalid_argument("Cannot set a 0 scale on progressive framebuffer");
    }

    setFrameSize({800, 600});
}

bool ProgressiveFrameHandler::commit()
{
    _lowRes.commit();
    return _highRes.commit();
}

void ProgressiveFrameHandler::setFrameSize(const Vector2ui &frameSize)
{
    auto lowResolution = frameSize / _scale;
    _lowRes.setFrameSize(lowResolution);
    _highRes.setFrameSize(frameSize);
}

void ProgressiveFrameHandler::setAccumulation(bool accumulation) noexcept
{
    _highRes.setAccumulation(accumulation);
}

void ProgressiveFrameHandler::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _lowRes.setFormat(frameBufferFormat);
    _highRes.setFormat(frameBufferFormat);
}

void ProgressiveFrameHandler::clear() noexcept
{
    _lowResFrame = true;
    _lowRes.clear();
    _highRes.clear();
}

void ProgressiveFrameHandler::incrementAccumFrames() noexcept
{
    (_lowResFrame ? _lowRes : _highRes).incrementAccumFrames();
}

size_t ProgressiveFrameHandler::getAccumulationFrameCount() const noexcept
{
    return _lowRes.getAccumulationFrameCount() + _highRes.getAccumulationFrameCount();
}

bool ProgressiveFrameHandler::hasNewAccumulationFrame() const noexcept
{
    return (_lowResFrame ? _lowRes : _highRes).hasNewAccumulationFrame();
}

void ProgressiveFrameHandler::resetNewAccumulationFrame() noexcept
{
    _lowRes.resetNewAccumulationFrame();
    _highRes.resetNewAccumulationFrame();
}

Image ProgressiveFrameHandler::getImage()
{
    return (std::exchange(_lowResFrame, false) ? _lowRes : _highRes).getImage();
}

const ospray::cpp::FrameBuffer &ProgressiveFrameHandler::getHandle() const noexcept
{
    return (_lowResFrame ? _lowRes : _highRes).getHandle();
}
}

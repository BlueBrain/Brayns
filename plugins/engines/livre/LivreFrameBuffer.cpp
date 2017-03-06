/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "LivreFrameBuffer.h"

#include <eq/fabric/pixelViewport.h>
#include <eq/image.h>

#include <livre/eq/Engine.h>

namespace brayns
{
LivreFrameBuffer::LivreFrameBuffer(const Vector2ui& frameSize,
                                   const FrameBufferFormat colorDepth,
                                   livre::Engine& livre)
    : FrameBuffer(frameSize, colorDepth, false)
    , _livre(livre)
{
    _livre.resize(frameSize);
}

LivreFrameBuffer::~LivreFrameBuffer()
{
}

void LivreFrameBuffer::resize(const Vector2ui& frameSize)
{
    if (_frameSize == frameSize)
        return;

    clear();
    _livre.resize(frameSize);
}

void LivreFrameBuffer::clear()
{
}

void LivreFrameBuffer::map()
{
}

void LivreFrameBuffer::unmap()
{
}

void LivreFrameBuffer::assign(const eq::Image& image)
{
    const auto& pvp = image.getPixelViewport();
    const Vector2ui newFrameSize = {uint32_t(pvp.w), uint32_t(pvp.h)};

    if (image.hasPixelData(eq::Frame::Buffer::color) &&
        newFrameSize == _frameSize)
        _colorBuffer =
            (uint8_t*)image.getPixelPointer(eq::Frame::Buffer::color);
    else
        _colorBuffer = nullptr;
    _frameSize = newFrameSize;
}
}

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

#include <brayns/engine/FrameBuffer.h>
#include <brayns/utils/image/ImageFlipper.h>

namespace
{
OSPFrameBufferFormat toOSPFrameBufferFormat(const brayns::PixelFormat frameBufferFormat)
{
    switch (frameBufferFormat)
    {
    case brayns::PixelFormat::SRGBA_I8:
        return OSP_FB_SRGBA;
    case brayns::PixelFormat::RGBA_I8:
        return OSP_FB_RGBA8;
    case brayns::PixelFormat::RGBA_F32:
        return OSP_FB_RGBA32F;
    }

    throw std::invalid_argument("Unknown PixelFormat");
    return OSP_FB_NONE;
}
} // namespace

namespace brayns
{
FrameBuffer::FrameBuffer()
{
    _recreate();
}

FrameBuffer::~FrameBuffer()
{
    unmap();
    ospRelease(_handle);
}

FrameBuffer::FrameBuffer(const FrameBuffer &o)
{
    *this = o;
}

FrameBuffer &FrameBuffer::operator=(const FrameBuffer &o)
{
    _frameSize = o._frameSize;
    _frameBufferFormat = o._frameBufferFormat;
    _accumulation = o._accumulation;
    _accumFrames = 0;

    _recreate();

    return *this;
}

FrameBuffer FrameBuffer::clone() const noexcept
{
    return FrameBuffer(*this);
}

void FrameBuffer::map()
{
    _colorBuffer = (uint8_t *)ospMapFrameBuffer(_handle, OSP_FB_COLOR);
}

void FrameBuffer::unmap()
{
    if (_colorBuffer)
    {
        ospUnmapFrameBuffer(_colorBuffer, _handle);
        _colorBuffer = nullptr;
    }
}

const uint8_t *FrameBuffer::getColorBuffer() const
{
    return _colorBuffer;
}

void FrameBuffer::commit()
{
    _recreate();
}

void FrameBuffer::setFrameSize(const Vector2ui &frameSize)
{
    if (glm::compMul(frameSize) == 0 || frameSize.x < 64 || frameSize.y < 64)
        throw std::invalid_argument("Frame size must be greather than or equal to 64x64");

    _updateValue(_frameSize, frameSize);
}

const Vector2ui &FrameBuffer::getFrameSize() const noexcept
{
    return _frameSize;
}

void FrameBuffer::setAccumulation(const bool accumulation) noexcept
{
   _updateValue(_accumulation, accumulation);
}

bool FrameBuffer::isAccumulating() const noexcept
{
    return _accumulation;
}

void FrameBuffer::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _updateValue(_frameBufferFormat, frameBufferFormat);
}

PixelFormat FrameBuffer::getFrameBufferFormat() const noexcept
{
    return _frameBufferFormat;
}

void FrameBuffer::clear() noexcept
{
    _accumFrames = 0;
    ospResetAccumulation(_handle);
}

void FrameBuffer::incrementAccumFrames() noexcept
{
    ++_accumFrames;
}

int32_t FrameBuffer::numAccumFrames() const noexcept
{
    return _accumFrames;
}

Image FrameBuffer::getImage()
{
    map();

    const auto colorBuffer = getColorBuffer();
    const auto &size = getFrameSize();

    ImageInfo info;

    info.width = size.x;
    info.height = size.y;
    info.channelCount = 4;
    info.channelSize = 1;

    auto data = reinterpret_cast<const char *>(colorBuffer);
    auto length = info.getSize();
    Image image(info, {data, length});
    ImageFlipper::flipVertically(image);

    unmap();

    return image;
}

OSPFrameBuffer FrameBuffer::handle() const noexcept
{
    return _handle;
}

void FrameBuffer::_recreate()
{
    unmap();

    if(_handle)
        ospRelease(_handle);

    const auto width = static_cast<int>(_frameSize.x);
    const auto height = static_cast<int>(_frameSize.y);

    const auto format = toOSPFrameBufferFormat(_frameBufferFormat);

    size_t channels = OSP_FB_COLOR;
    if (_accumulation)
        channels |= OSP_FB_ACCUM;

    _handle = ospNewFrameBuffer(width, height, format, channels);

    ospCommit(_handle);

    clear();
}
} // namespace brayns

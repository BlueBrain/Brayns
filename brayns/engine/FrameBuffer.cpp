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

#include "FrameBuffer.h"

#include <brayns/utils/image/ImageFlipper.h>
#include <ospray/ospray_cpp/Data.h>

namespace
{
class OsprayFrameBufferFormat
{
public:
    static OSPFrameBufferFormat fromPixelFormat(const brayns::PixelFormat frameBufferFormat)
    {
        switch (frameBufferFormat)
        {
        case brayns::PixelFormat::StandardRgbaI8:
            return OSP_FB_SRGBA;
        case brayns::PixelFormat::RgbaI8:
            return OSP_FB_RGBA8;
        case brayns::PixelFormat::RgbaF32:
            return OSP_FB_RGBA32F;
        }

        throw std::invalid_argument("Unknown PixelFormat");
        return OSP_FB_NONE;
    }
};
} // namespace

namespace brayns
{
void FrameBuffer::map()
{
    _colorBuffer = static_cast<uint8_t *>(_osprayFramebuffer.map(OSP_FB_COLOR));
}

void FrameBuffer::unmap()
{
    _osprayFramebuffer.unmap(_colorBuffer);
    _colorBuffer = nullptr;
}

const uint8_t *FrameBuffer::getColorBuffer() const
{
    return _colorBuffer;
}

bool FrameBuffer::commit()
{
    if (!isModified())
    {
        return false;
    }

    unmap();

    const auto width = static_cast<int>(_frameSize.x);
    const auto height = static_cast<int>(_frameSize.y);

    const auto format = OsprayFrameBufferFormat::fromPixelFormat(_frameBufferFormat);

    size_t channels = OSP_FB_COLOR;
    if (_accumulation)
    {
        channels |= OSP_FB_ACCUM;
    }

    _osprayFramebuffer = ospray::cpp::FrameBuffer(width, height, format, channels);

    auto operations = _operationManager.getOperationHandles();
    if (!operations.empty())
    {
        _osprayFramebuffer.setParam("imageOperation", ospray::cpp::CopiedData(operations));
    }

    _osprayFramebuffer.commit();

    clear();

    resetModified();

    return true;
}

void FrameBuffer::setFrameSize(const Vector2ui &frameSize)
{
    if (glm::compMul(frameSize) == 0 || frameSize.x < 64 || frameSize.y < 64)
    {
        throw std::invalid_argument("Frame size must be greather than or equal to 64x64");
    }

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
    _osprayFramebuffer.clear();
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
    info.channelSize = PixelFormatChannelByteSize::get(_frameBufferFormat);

    auto data = reinterpret_cast<const char *>(colorBuffer);
    auto length = info.getSize();
    Image image(info, {data, length});
    ImageFlipper::flipVertically(image);

    unmap();

    return image;
}

const ospray::cpp::FrameBuffer &FrameBuffer::getOsprayFramebuffer() const noexcept
{
    return _osprayFramebuffer;
}

ImageOperationManager &FrameBuffer::getOperationsManager() noexcept
{
    return _operationManager;
}
} // namespace brayns

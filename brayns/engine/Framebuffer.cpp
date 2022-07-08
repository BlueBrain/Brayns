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

#include "Framebuffer.h"

#include <brayns/utils/image/ImageFlipper.h>
#include <ospray/ospray_cpp/Data.h>

namespace
{
struct FrameBufferParameters
{
    inline static const std::string operations = "imageOperation";
};

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
void Framebuffer::map()
{
    _colorBuffer = static_cast<uint8_t *>(_osprayFramebuffer.map(OSP_FB_COLOR));
}

void Framebuffer::unmap()
{
    if (_colorBuffer)
    {
        _osprayFramebuffer.unmap(_colorBuffer);
        _colorBuffer = nullptr;
    }
}

const uint8_t *Framebuffer::getColorBuffer() const
{
    return _colorBuffer;
}

bool Framebuffer::commit()
{
    if (!_flag)
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
        _osprayFramebuffer.setParam(FrameBufferParameters::operations, ospray::cpp::CopiedData(operations));
    }

    _osprayFramebuffer.commit();

    clear();

    _flag = false;

    return true;
}

void Framebuffer::setFrameSize(const Vector2ui &frameSize)
{
    if (glm::compMul(frameSize) == 0 || frameSize.x < 64 || frameSize.y < 64)
    {
        throw std::invalid_argument("Frame size must be greather than or equal to 64x64");
    }

    _flag.update(_frameSize, frameSize);
}

const Vector2ui &Framebuffer::getFrameSize() const noexcept
{
    return _frameSize;
}

void Framebuffer::setAccumulation(const bool accumulation) noexcept
{
    _flag.update(_accumulation, accumulation);
}

bool Framebuffer::isAccumulating() const noexcept
{
    return _accumulation;
}

void Framebuffer::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _flag.update(_frameBufferFormat, frameBufferFormat);
}

PixelFormat Framebuffer::getFrameBufferFormat() const noexcept
{
    return _frameBufferFormat;
}

void Framebuffer::clear() noexcept
{
    _accumFrames = 0;
    _osprayFramebuffer.clear();
}

void Framebuffer::incrementAccumFrames() noexcept
{
    ++_accumFrames;
    _newAccumulationFrame = true;
}

int32_t Framebuffer::numAccumFrames() const noexcept
{
    return _accumFrames;
}

bool Framebuffer::hasNewAccumulationFrame() const noexcept
{
    return _newAccumulationFrame;
}

void Framebuffer::resetNewAccumulationFrame() noexcept
{
    _newAccumulationFrame = false;
}

Image Framebuffer::getImage()
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

const ospray::cpp::FrameBuffer &Framebuffer::getOsprayFramebuffer() const noexcept
{
    return _osprayFramebuffer;
}

ImageOperationManager &Framebuffer::getOperationsManager() noexcept
{
    return _operationManager;
}
} // namespace brayns

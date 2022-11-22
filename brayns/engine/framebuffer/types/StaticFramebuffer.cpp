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

#include "StaticFramebuffer.h"

#include <brayns/engine/framebuffer/ToneMapping.h>
#include <brayns/utils/image/ImageFlipper.h>
#include <ospray/ospray_cpp/Data.h>

namespace
{
/**
 * @brief Returns the size, in bytes, of each color channel of a given pixel format
 */
class PixelFormatChannelByteSize
{
public:
    static size_t get(brayns::PixelFormat format)
    {
        switch (format)
        {
        case brayns::PixelFormat::RgbaF32:
            return 4;
        default:
            return 1;
        }
    }
};

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
}

namespace brayns
{
void StaticFramebuffer::map()
{
    _colorBuffer = static_cast<uint8_t *>(_handle.map(OSP_FB_COLOR));
}

void StaticFramebuffer::unmap()
{
    if (_colorBuffer)
    {
        _handle.unmap(_colorBuffer);
        _colorBuffer = nullptr;
    }
}

const uint8_t *StaticFramebuffer::getColorBuffer() const
{
    return _colorBuffer;
}

bool StaticFramebuffer::commit()
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

    _handle = ospray::cpp::FrameBuffer(width, height, format, channels);

    auto toneMapping = ToneMappingFactory::create();
    _handle.setParam(FrameBufferParameters::operations, ospray::cpp::CopiedData(&toneMapping, 1));

    _handle.commit();

    clear();
    _flag = false;
    return true;
}

void StaticFramebuffer::setFrameSize(const Vector2ui &frameSize)
{
    if (glm::compMul(frameSize) == 0 || frameSize.x < 64 || frameSize.y < 64)
    {
        throw std::invalid_argument("Frame size must be greather than or equal to 64x64");
    }

    _flag.update(_frameSize, frameSize);
}

const Vector2ui &StaticFramebuffer::getFrameSize() const noexcept
{
    return _frameSize;
}

float StaticFramebuffer::getAspectRatio() const noexcept
{
    return static_cast<float>(_frameSize.x) / static_cast<float>(_frameSize.y);
}

void StaticFramebuffer::setAccumulation(const bool accumulation) noexcept
{
    _flag.update(_accumulation, accumulation);
}

bool StaticFramebuffer::isAccumulating() const noexcept
{
    return _accumulation;
}

void StaticFramebuffer::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _flag.update(_frameBufferFormat, frameBufferFormat);
}

PixelFormat StaticFramebuffer::getFrameBufferFormat() const noexcept
{
    return _frameBufferFormat;
}

void StaticFramebuffer::clear() noexcept
{
    _accumFrames = 0;
    _handle.clear();
}

void StaticFramebuffer::incrementAccumFrames() noexcept
{
    ++_accumFrames;
    _newAccumulationFrame = true;
}

int32_t StaticFramebuffer::numAccumFrames() const noexcept
{
    return _accumFrames;
}

bool StaticFramebuffer::hasNewAccumulationFrame() const noexcept
{
    return _newAccumulationFrame;
}

void StaticFramebuffer::resetNewAccumulationFrame() noexcept
{
    _newAccumulationFrame = false;
}

Image StaticFramebuffer::getImage()
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

const ospray::cpp::FrameBuffer &StaticFramebuffer::getHandle() const noexcept
{
    return _handle;
}
}

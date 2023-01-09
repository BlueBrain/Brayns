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

#include "StaticFrameHandler.h"

#include <brayns/engine/framebuffer/ToneMapping.h>
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

class FrameStream
{
public:
    explicit FrameStream(ospray::cpp::FrameBuffer &handle)
        : _handle(handle)
        , _channelHandle(nullptr)
    {
    }

    ~FrameStream()
    {
        close();
    }

    template<typename Type>
    Type *openAs(OSPFrameBufferChannel channel)
    {
        if (!_channelHandle)
        {
            _channelHandle = _handle.map(channel);
        }
        return static_cast<Type *>(_channelHandle);
    }

    void close()
    {
        if (!_channelHandle)
        {
            return;
        }
        _handle.unmap(_channelHandle);
        _channelHandle = nullptr;
    }

private:
    ospray::cpp::FrameBuffer &_handle;
    void *_channelHandle;
};
}

namespace brayns
{
bool StaticFrameHandler::commit()
{
    if (!_flag)
    {
        return false;
    }

    auto width = static_cast<int>(_frameSize.x);
    auto height = static_cast<int>(_frameSize.y);
    auto format = OsprayFrameBufferFormat::fromPixelFormat(_format);
    auto channels = _accumulation ? OSP_FB_COLOR | OSP_FB_ACCUM : OSP_FB_COLOR;
    _handle = ospray::cpp::FrameBuffer(width, height, format, channels);

    auto toneMapping = ToneMappingFactory::create();
    _handle.setParam(FrameBufferParameters::operations, ospray::cpp::CopiedData(&toneMapping, 1));

    _handle.commit();

    clear();
    _flag = false;
    return true;
}

void StaticFrameHandler::setFrameSize(const Vector2ui &frameSize)
{
    if (glm::compMul(frameSize) == 0 || glm::compMin(frameSize) < 64)
    {
        throw std::invalid_argument("Frame size must be greather than or equal to 64x64");
    }

    _flag.update(_frameSize, frameSize);
}

void StaticFrameHandler::setAccumulation(bool accumulation) noexcept
{
    _flag.update(_accumulation, accumulation);
}

void StaticFrameHandler::setFormat(PixelFormat frameBufferFormat) noexcept
{
    _flag.update(_format, frameBufferFormat);
}

void StaticFrameHandler::clear() noexcept
{
    _accumFrames = 0;
    _handle.clear();
}

void StaticFrameHandler::incrementAccumFrames() noexcept
{
    ++_accumFrames;
    _newAccumulationFrame = true;
}

size_t StaticFrameHandler::getAccumulationFrameCount() const noexcept
{
    return _accumFrames;
}

bool StaticFrameHandler::hasNewAccumulationFrame() const noexcept
{
    return _newAccumulationFrame;
}

void StaticFrameHandler::resetNewAccumulationFrame() noexcept
{
    _newAccumulationFrame = false;
}

Image StaticFrameHandler::getImage()
{
    auto colorStream = FrameStream(_handle);
    auto colorBuffer = colorStream.openAs<uint8_t>(OSP_FB_COLOR);

    ImageInfo info;

    info.width = _frameSize.x;
    info.height = _frameSize.y;
    info.channelCount = 4;
    info.channelSize = PixelFormatChannelByteSize::get(_format);

    auto data = reinterpret_cast<const char *>(colorBuffer);
    auto length = info.getSize();
    return Image(info, {data, length});
}

const ospray::cpp::FrameBuffer &StaticFrameHandler::getHandle() const noexcept
{
    return _handle;
}
}
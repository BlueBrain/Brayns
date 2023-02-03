/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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
struct FrameBufferParameters
{
    inline static const std::string operations = "imageOperation";
};

/**
 * @brief Returns the size, in bytes, of each color channel of a given pixel format
 */
class PixelFormatChannelByteSize
{
public:
    static size_t get(brayns::FramebufferChannel channel, brayns::PixelFormat format)
    {
        if (channel == brayns::FramebufferChannel::Color)
        {
            if (format == brayns::PixelFormat::RgbaF32)
            {
                return 4;
            }
            return 1;
        }

        return 4;
    }
};

class FramebufferChannelCount
{
public:
    static size_t get(brayns::FramebufferChannel channel)
    {
        switch (channel)
        {
        case brayns::FramebufferChannel::Color:
            return 4;
        case brayns::FramebufferChannel::Albedo:
        case brayns::FramebufferChannel::Normal:
            return 3;
        case brayns::FramebufferChannel::Depth:
            return 1;
        }

        throw std::runtime_error("Unsupported framebuffer channel");
    }
};

class FramebufferDataType
{
public:
    static brayns::ImageDataType get(brayns::FramebufferChannel channel)
    {
        switch (channel)
        {
        case brayns::FramebufferChannel::Color:
            return brayns::ImageDataType::UnsignedInt;
        case brayns::FramebufferChannel::Albedo:
        case brayns::FramebufferChannel::Normal:
        case brayns::FramebufferChannel::Depth:
            return brayns::ImageDataType::Float;
        }

        throw std::runtime_error("Unsupported framebuffer channel");
    }
};

class ValidFramebufferChannel
{
public:
    static void check(uint32_t mask, brayns::FramebufferChannel channel)
    {
        if (!(mask & static_cast<uint32_t>(channel)))
        {
            throw std::invalid_argument("Requested framebuffer channel is not available");
        }
    }
};

class OsprayFrameBufferFormat
{
public:
    static OSPFrameBufferFormat fromPixelFormat(brayns::PixelFormat frameBufferFormat)
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

class OsprayFrameBufferChannel
{
public:
    static uint32_t build(const std::vector<brayns::FramebufferChannel> &channels)
    {
        auto channelMask = 0u;

        for (auto channel : channels)
        {
            channelMask |= static_cast<uint32_t>(channel);
        }

        if (channelMask == 0u)
        {
            throw std::runtime_error("Framebuffer without channels is not allowed");
        }

        return channelMask;
    }
};

class FrameStream
{
public:
    explicit FrameStream(const ospray::cpp::FrameBuffer &handle)
        : _handle(handle)
        , _channelHandle(nullptr)
    {
    }

    ~FrameStream()
    {
        close();
    }

    template<typename Type>
    Type *openAs(brayns::FramebufferChannel channel)
    {
        if (!_channelHandle)
        {
            _channelHandle = _handle.map(static_cast<OSPFrameBufferChannel>(channel));
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
    const ospray::cpp::FrameBuffer &_handle;
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
    auto channels = _accumulation ? _channelMask & OSP_FB_ACCUM : _channelMask;
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

void StaticFrameHandler::setChannels(const std::vector<FramebufferChannel> &channels) noexcept
{
    _flag.update(_channelMask, OsprayFrameBufferChannel::build(channels));
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

Image StaticFrameHandler::getImage(FramebufferChannel channel)
{
    ValidFramebufferChannel::check(_channelMask, channel);

    auto stream = FrameStream(_handle);
    auto buffer = stream.openAs<uint8_t>(channel);

    ImageInfo info;

    info.width = _frameSize.x;
    info.height = _frameSize.y;
    info.dataType = FramebufferDataType::get(channel);
    info.channelCount = FramebufferChannelCount::get(channel);
    info.channelSize = PixelFormatChannelByteSize::get(channel, _format);

    auto data = reinterpret_cast<const char *>(buffer);
    auto length = info.getSize();
    return Image(info, {data, length});
}

const ospray::cpp::FrameBuffer &StaticFrameHandler::getHandle() const noexcept
{
    return _handle;
}
}

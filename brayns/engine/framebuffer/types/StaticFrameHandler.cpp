/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <algorithm>
#include <unordered_set>

namespace
{
struct FrameBufferParameters
{
    static inline const std::string operations = "imageOperation";
};

/**
 * @brief Returns the size, in bytes, of each color channel of a given pixel format
 */
class PixelFormatChannelByteSize
{
public:
    static size_t get(brayns::FramebufferChannel channel, brayns::PixelFormat format)
    {
        if (channel != brayns::FramebufferChannel::Color)
        {
            return 4;
        }

        if (format == brayns::PixelFormat::RgbaF32)
        {
            return 4;
        }

        return 1;
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
    static brayns::ImageDataType get(brayns::FramebufferChannel channel, brayns::PixelFormat format)
    {
        switch (channel)
        {
        case brayns::FramebufferChannel::Color:
        {
            switch (format)
            {
            case brayns::PixelFormat::RgbaF32:
                return brayns::ImageDataType::Float;
            default:
                return brayns::ImageDataType::UnsignedInt;
            }
        }
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
    static bool check(const std::vector<brayns::FramebufferChannel> &channels, brayns::FramebufferChannel channel)
    {
        return std::find(channels.begin(), channels.end(), channel) != channels.end();
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
    static bool hasDuplicates(const std::vector<brayns::FramebufferChannel> &channels)
    {
        auto uniques = std::unordered_set<brayns::FramebufferChannel>(channels.begin(), channels.end());
        return uniques.size() < channels.size();
    }

    static uint32_t buildMask(const std::vector<brayns::FramebufferChannel> &channels, bool accumulation)
    {
        auto channelMask = 0u;

        for (auto channel : channels)
        {
            channelMask |= static_cast<uint32_t>(channel);
        }

        if (accumulation)
        {
            channelMask |= OSP_FB_ACCUM;
        }

        assert(channelMask != 0u);

        return channelMask;
    }
};

class FrameStream
{
public:
    explicit FrameStream(const ospray::cpp::FrameBuffer &handle):
        _handle(handle),
        _channelHandle(nullptr)
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
    auto channels = OsprayFrameBufferChannel::buildMask(_channels, _accumulation);
    _handle = ospray::cpp::FrameBuffer(width, height, format, channels);

    if (_toneMapping)
    {
        auto toneMapping = ToneMappingFactory::create();
        _handle.setParam(FrameBufferParameters::operations, ospray::cpp::CopiedData(&toneMapping, 1));
    }

    _handle.commit();

    clear();
    _flag = false;
    return true;
}

void StaticFrameHandler::setFrameSize(const Vector2ui &frameSize)
{
    if (math::reduce_mul(frameSize) == 0 || math::reduce_min(frameSize) < 64)
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
    assert(!OsprayFrameBufferChannel::hasDuplicates(channels));
    _flag.update(_channels, channels);
}

void StaticFrameHandler::setToneMappingEnabled(bool enabled) noexcept
{
    _flag.update(_toneMapping, enabled);
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
    assert(ValidFramebufferChannel::check(_channels, channel));

    auto stream = FrameStream(_handle);
    auto buffer = stream.openAs<uint8_t>(channel);

    ImageInfo info;

    info.width = _frameSize.x;
    info.height = _frameSize.y;
    info.dataType = FramebufferDataType::get(channel, _format);
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

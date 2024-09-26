/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "Encoding.h"

#include <cassert>
#include <cmath>
#include <concepts>
#include <span>
#include <type_traits>

#include <brayns/core/codecs/Common.h>
#include <brayns/core/codecs/ExrCodec.h>
#include <brayns/core/codecs/PngCodec.h>

#include "Binary.h"

namespace
{
using namespace brayns;

template<typename T>
std::string readChannelAs(UserFramebuffer &framebuffer, FramebufferChannel channel)
{
    const auto &size = framebuffer.settings.resolution;

    auto data = framebuffer.value.map(channel);
    auto itemCount = reduceMultiply(size);

    auto items = std::span<const T>(data.as<T>(), itemCount);

    return composeRangeToBinary(items);
}

template<typename T>
concept Encoder = std::invocable<T, const ImageView &> && std::same_as<std::string, std::invoke_result_t<T, const ImageView &>>;

std::string encodeColorChannel(const FramebufferData &data, const Size2 &size, Encoder auto &&encoder)
{
    auto format = ImageFormat::Rgba8;
    return encoder(ImageView{data.get(), size, format});
}

std::string convertAndEncodeColorChannel(const FramebufferData &data, const Size2 &size, Encoder auto &&encoder)
{
    const auto *items = data.as<Color4>();
    auto itemCount = reduceMultiply(size);
    auto converted = convertTo8Bit(std::span<const Color4>(items, itemCount));
    auto format = ImageFormat::Rgba8;

    return encoder(ImageView{converted.data(), size, format});
}

std::string convertAndEncodeAlbedoChannel(const FramebufferData &data, const Size2 &size, Encoder auto &&encoder)
{
    const auto *items = data.as<Color3>();
    auto itemCount = reduceMultiply(size);
    auto converted = convertTo8Bit(std::span<const Color3>(items, itemCount));
    auto format = ImageFormat::Rgb8;

    return encoder(ImageView{converted.data(), size, format});
}

std::string encodeChannelToJpegOrPng(UserFramebuffer &framebuffer, JpegChannel channel, Encoder auto &&encoder)
{
    const auto &size = framebuffer.settings.resolution;
    auto format = framebuffer.settings.format;

    auto data = framebuffer.value.map(static_cast<FramebufferChannel>(channel));

    if (channel == JpegChannel::Albedo)
    {
        return convertAndEncodeAlbedoChannel(data, size, encoder);
    }

    assert(channel == JpegChannel::Color);

    switch (format)
    {
    case FramebufferFormat::Rgba8:
    case FramebufferFormat::Srgba8:
        return encodeColorChannel(data, size, encoder);
    case FramebufferFormat::Rgba32F:
        return convertAndEncodeColorChannel(data, size, encoder);
    default:
        throw std::invalid_argument("Invalid Framebuffer format");
    };
}

using ExrMappedData = std::variant<FramebufferData, std::vector<Color4>>;

const void *getExrChannelPtr(const FramebufferData &data)
{
    return data.get();
}

const void *getExrChannelPtr(const std::vector<Color4> &data)
{
    return data.data();
}

const void *getExrChannelPtr(const ExrMappedData &data)
{
    return std::visit([](const auto &value) { return getExrChannelPtr(value); }, data);
}

std::vector<ExrMappedData> mapExrChannels(
    Framebuffer &framebuffer,
    std::size_t pixelCount,
    FramebufferFormat format,
    const std::set<FramebufferChannel> &channels)
{
    auto result = std::vector<ExrMappedData>();
    result.reserve(channels.size());

    for (auto channel : channels)
    {
        auto data = framebuffer.map(channel);

        if (format != FramebufferFormat::Rgba32F && channel == FramebufferChannel::Color)
        {
            auto pixels = std::string_view(data.as<char>(), 4 * pixelCount);
            auto items = convertToFloat<4>(pixels);

            result.push_back(std::move(items));

            continue;
        }

        result.push_back(std::move(data));
    }

    return result;
}

std::string getExrChannelName(const char *layer, const char *channel)
{
    if (layer[0] == '\0')
    {
        return channel;
    }

    return layer + std::string(".") + channel;
}

template<typename T, int S>
std::vector<ExrChannel> splitExrChannels(const Vector<T, S> *items, const char *layer, const std::array<const char *, std::size_t(S)> &channels)
{
    constexpr auto dataType = ExrDataType::F32;
    constexpr auto stride = sizeof(Vector<T, S>);

    auto result = std::vector<ExrChannel>();
    result.reserve(channels.size());

    for (auto i = std::size_t(0); i < channels.size(); ++i)
    {
        auto name = getExrChannelName(layer, channels[i]);

        result.push_back({std::move(name), &items[0][i], dataType, stride});
    }

    return result;
}

std::vector<ExrChannel> createExrChannels(const void *data, FramebufferChannel channel)
{
    switch (channel)
    {
    case FramebufferChannel::Color:
        return splitExrChannels(static_cast<const Color4 *>(data), "", {"R", "G", "B", "A"});
    case FramebufferChannel::Depth:
        return {{"Z", data}};
    case FramebufferChannel::Normal:
        return splitExrChannels(static_cast<const Vector3 *>(data), "normal", {"X", "Y", "Z"});
    case FramebufferChannel::Albedo:
        return splitExrChannels(static_cast<const Color3 *>(data), "albedo", {"R", "G", "B"});
    case FramebufferChannel::PrimitiveId:
        return {{"primitive.ID", data, ExrDataType::U32}};
    case FramebufferChannel::ModelId:
        return {{"model.ID", data, ExrDataType::U32}};
    case FramebufferChannel::InstanceId:
        return {{"instance.ID", data, ExrDataType::U32}};
    default:
        throw std::invalid_argument("Invalid framebuffer channel");
    }
}

std::vector<ExrChannel> createExrChannels(const std::vector<ExrMappedData> &datas, const std::set<FramebufferChannel> &channels)
{
    assert(datas.size() == channels.size());

    auto result = std::vector<ExrChannel>();

    auto i = std::size_t(0);

    for (auto channel : channels)
    {
        const auto &data = datas[i++];
        const auto *ptr = getExrChannelPtr(data);

        auto exrChannels = createExrChannels(ptr, channel);

        for (auto &exrChannel : exrChannels)
        {
            result.push_back(std::move(exrChannel));
        }
    }

    return result;
}
}

namespace brayns
{
std::string readChannel(UserFramebuffer &framebuffer, FramebufferChannel channel)
{
    switch (channel)
    {
    case FramebufferChannel::Color:
        return readChannelAs<Color4>(framebuffer, channel);
    case FramebufferChannel::Depth:
        return readChannelAs<float>(framebuffer, channel);
    case FramebufferChannel::Normal:
        return readChannelAs<Vector3>(framebuffer, channel);
    case FramebufferChannel::Albedo:
        return readChannelAs<Color3>(framebuffer, channel);
    case FramebufferChannel::PrimitiveId:
        return readChannelAs<std::uint32_t>(framebuffer, channel);
    case FramebufferChannel::ModelId:
        return readChannelAs<std::uint32_t>(framebuffer, channel);
    case FramebufferChannel::InstanceId:
        return readChannelAs<std::uint32_t>(framebuffer, channel);
    default:
        throw std::invalid_argument("Invalid raw channel");
    };
}

std::string readChannelAsJpeg(UserFramebuffer &framebuffer, JpegChannel channel, const JpegSettings &settings)
{
    auto encoder = [&](const auto &image) { return encodeJpeg(image, settings); };
    return encodeChannelToJpegOrPng(framebuffer, channel, encoder);
}

std::string readChannelAsPng(UserFramebuffer &framebuffer, PngChannel channel)
{
    auto encoder = [&](const auto &image) { return encodePng(image); };
    return encodeChannelToJpegOrPng(framebuffer, channel, encoder);
}

std::string readChannelsAsExr(UserFramebuffer &framebuffer, const std::set<FramebufferChannel> &channels)
{
    const auto &size = framebuffer.settings.resolution;
    auto format = framebuffer.settings.format;

    auto pixelCount = reduceMultiply(size);
    auto datas = mapExrChannels(framebuffer.value, pixelCount, format, channels);

    auto exrChannels = createExrChannels(datas, channels);

    return encodeExr({size, std::move(exrChannels)});
}
}

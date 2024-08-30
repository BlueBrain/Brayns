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

#include "ImageEndpoints.h"

#include <cassert>
#include <cmath>
#include <concepts>
#include <span>
#include <type_traits>

#include <fmt/format.h>

#include <brayns/core/codecs/Common.h>
#include <brayns/core/codecs/ExrCodec.h>
#include <brayns/core/codecs/JpegCodec.h>
#include <brayns/core/codecs/PngCodec.h>
#include <brayns/core/jsonrpc/Errors.h>

#include "FramebufferEndpoints.h"
#include "common/Binary.h"

namespace brayns
{
void checkChannelInFramebuffer(const UserFramebuffer &framebuffer, FramebufferChannel channel)
{
    if (framebuffer.settings.channels.contains(channel))
    {
        return;
    }

    const auto &name = getEnumName(channel);
    throw InvalidParams(fmt::format("The given framebuffer was created without the channel {}", name));
}

struct RawImageParams
{
    FramebufferChannel channel;
};

template<>
struct JsonObjectReflector<RawImageParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RawImageParams>();
        builder.field("channel", [](auto &object) { return &object.channel; })
            .description("Channel of the framebuffer to encode");
        return builder.build();
    }
};

template<typename T>
std::string readChannelAs(UserFramebuffer &framebuffer, FramebufferChannel channel)
{
    auto &deviceObject = framebuffer.deviceObject;
    const auto &size = framebuffer.settings.resolution;

    auto data = deviceObject.map(channel);
    auto itemCount = reduceMultiply(size);

    auto items = std::span<const T>(data.as<T>(), itemCount);

    return composeRangeToBinary(items);
}

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

enum class JpegChannel
{
    Color = int(FramebufferChannel::Color),
    Albedo = int(FramebufferChannel::Albedo),
};

template<>
struct EnumReflector<JpegChannel>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<JpegChannel>();
        builder.field("Color", JpegChannel::Color).description("RGBA (PNG) or RGB (JPEG) color as uint8");
        builder.field("Albedo", JpegChannel::Albedo).description("RGB color as uint8");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<JpegSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JpegSettings>();
        builder.field("quality", [](auto &object) { return &object.quality; })
            .description("JPEG quality, 1 = worst quality / best compression, 100 = best quality / worst compression")
            .defaultValue(100)
            .minimum(1)
            .maximum(100);
        return builder.build();
    }
};

struct JpegImageParams
{
    JpegChannel channel;
    JpegSettings settings;
};

template<>
struct JsonObjectReflector<JpegImageParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JpegImageParams>();
        builder.field("channel", [](auto &object) { return &object.channel; })
            .description("Channel of the framebuffer to encode");
        builder.field("settings", [](auto &object) { return &object.settings; }).description("JPEG encoder settings");
        return builder.build();
    }
};

template<typename T>
concept Encoder =
    std::invocable<T, const ImageView &> && std::same_as<std::string, std::invoke_result_t<T, const ImageView &>>;

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
    auto &deviceObject = framebuffer.deviceObject;
    const auto &size = framebuffer.settings.resolution;
    auto format = framebuffer.settings.format;

    auto data = deviceObject.map(static_cast<FramebufferChannel>(channel));

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

std::string encodeChannelToJpeg(UserFramebuffer &framebuffer, JpegChannel channel, const JpegSettings &settings)
{
    auto encoder = [&](const auto &image) { return encodeJpeg(image, settings); };
    return encodeChannelToJpegOrPng(framebuffer, channel, encoder);
}

using PngChannel = JpegChannel;

struct PngImageParams
{
    PngChannel channel;
};

template<>
struct JsonObjectReflector<PngImageParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PngImageParams>();
        builder.field("channel", [](auto &object) { return &object.channel; })
            .description("Channel of the framebuffer to encode");
        return builder.build();
    }
};

std::string encodeChannelToPng(UserFramebuffer &framebuffer, PngChannel channel)
{
    auto encoder = [&](const auto &image) { return encodePng(image); };
    return encodeChannelToJpegOrPng(framebuffer, channel, encoder);
}

struct ExrImageParams
{
    std::set<FramebufferChannel> channels;
};

template<>
struct JsonObjectReflector<ExrImageParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ExrImageParams>();
        builder.field("channels", [](auto &object) { return &object.channels; })
            .description("Channels of the framebuffer to encode");
        return builder.build();
    }
};

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
    const auto dot = std::string(".");

    if (layer[0] == '\0')
    {
        return channel;
    }

    return layer + dot + channel;
}

template<typename T, int S>
std::vector<ExrChannel> splitExrChannels(
    const Vector<T, S> *items,
    const char *layer,
    const std::array<const char *, std::size_t(S)> &channels)
{
    constexpr auto dataType = ExrDataType::F32;
    constexpr auto stride = sizeof(Vector<T, S>);

    const auto dot = std::string(".");

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

std::vector<ExrChannel> createExrChannels(
    const std::vector<ExrMappedData> &datas,
    const std::set<FramebufferChannel> &channels)
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

std::string encodeChannelsToExr(UserFramebuffer &framebuffer, const std::set<FramebufferChannel> &channels)
{
    auto &deviceObject = framebuffer.deviceObject;
    const auto &size = framebuffer.settings.resolution;
    auto format = framebuffer.settings.format;

    auto pixelCount = reduceMultiply(size);
    auto datas = mapExrChannels(deviceObject, pixelCount, format, channels);

    auto exrChannels = createExrChannels(datas, channels);

    return encodeExr({size, std::move(exrChannels)});
}

template<ReflectedJson T>
struct ImageParams
{
    ObjectId id;
    T settings;
};

template<ReflectedJson T>
struct JsonObjectReflector<ImageParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ImageParams<T>>();
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the framebuffer to read");
        builder.field("settings", [](auto &object) { return &object.settings; })
            .description("Settings to encode the framebuffer content");
        return builder.build();
    }
};

Result<NullJson> readFramebuffer(LockedObjects &locked, const ImageParams<RawImageParams> &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            checkChannelInFramebuffer(framebuffer, params.settings.channel);

            auto data = readChannel(framebuffer, params.settings.channel);

            return Result<NullJson>{{}, std::move(data)};
        });
}

Result<NullJson> readFramebufferAsJpeg(LockedObjects &locked, const ImageParams<JpegImageParams> &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            auto channel = static_cast<FramebufferChannel>(params.settings.channel);
            checkChannelInFramebuffer(framebuffer, channel);

            auto data = encodeChannelToJpeg(framebuffer, params.settings.channel, params.settings.settings);

            return Result<NullJson>{{}, std::move(data)};
        });
}

Result<NullJson> readFramebufferAsPng(LockedObjects &locked, const ImageParams<PngImageParams> &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            auto channel = static_cast<FramebufferChannel>(params.settings.channel);
            checkChannelInFramebuffer(framebuffer, channel);

            auto data = encodeChannelToPng(framebuffer, params.settings.channel);

            return Result<NullJson>{{}, std::move(data)};
        });
}

Result<NullJson> readFramebufferAsExr(LockedObjects &locked, const ImageParams<ExrImageParams> &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            for (auto channel : params.settings.channels)
            {
                checkChannelInFramebuffer(framebuffer, channel);
            }

            auto data = encodeChannelsToExr(framebuffer, params.settings.channels);

            return Result<NullJson>{{}, std::move(data)};
        });
}

void addImageEndpoints(ApiBuilder &builder, LockedObjects &objects)
{
    builder
        .endpoint(
            "readFramebuffer",
            [&](ImageParams<RawImageParams> params) { return readFramebuffer(objects, params); })
        .description("Read a framebuffer channel and return it in the binary part of the message");

    builder
        .endpoint(
            "readFramebufferAsJpeg",
            [&](ImageParams<JpegImageParams> params) { return readFramebufferAsJpeg(objects, params); })
        .description("Read a framebuffer channel and return it encoded as JPEG in the binary part of the message");

    builder
        .endpoint(
            "readFramebufferAsPng",
            [&](ImageParams<PngImageParams> params) { return readFramebufferAsPng(objects, params); })
        .description("Read a framebuffer channel and return it encoded as PNG in the binary part of the message");

    builder
        .endpoint(
            "readFramebufferAsExr",
            [&](ImageParams<ExrImageParams> params) { return readFramebufferAsExr(objects, params); })
        .description("Read framebuffer channels and return them encoded as EXR in the binary part of the message");
}
}

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

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Errors.h>

#include <brayns/core/objects/common/Encoding.h>

namespace
{
using namespace brayns;

void checkChannelInFramebuffer(const UserFramebuffer &framebuffer, FramebufferChannel channel)
{
    if (framebuffer.settings.channels.contains(channel))
    {
        return;
    }

    const auto &name = getEnumName(channel);
    throw InvalidParams(fmt::format("The framebuffer has no channels '{}'", name));
}
}

namespace brayns
{
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
        builder.field("channel", [](auto &object) { return &object.channel; }).description("Channel of the framebuffer to encode");
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
        builder.field("channel", [](auto &object) { return &object.channel; }).description("Channel of the framebuffer to encode");
        builder.field("settings", [](auto &object) { return &object.settings; }).description("JPEG encoder settings");
        return builder.build();
    }
};

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
        builder.field("channel", [](auto &object) { return &object.channel; }).description("Channel of the framebuffer to encode");
        return builder.build();
    }
};

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
        builder.field("channels", [](auto &object) { return &object.channels; }).description("Channels of the framebuffer to encode");
        return builder.build();
    }
};

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
        builder.field("settings", [](auto &object) { return &object.settings; }).description("Settings to encode the framebuffer content");
        return builder.build();
    }
};

Result<NullJson> readFramebuffer(ObjectManager &objects, const ImageParams<RawImageParams> &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);

    checkChannelInFramebuffer(framebuffer, params.settings.channel);

    auto data = readChannel(framebuffer, params.settings.channel);

    return Result<NullJson>{{}, std::move(data)};
}

Result<NullJson> readFramebufferAsJpeg(ObjectManager &objects, const ImageParams<JpegImageParams> &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);

    auto channel = static_cast<FramebufferChannel>(params.settings.channel);
    checkChannelInFramebuffer(framebuffer, channel);

    auto data = readChannelAsJpeg(framebuffer, params.settings.channel, params.settings.settings);

    return Result<NullJson>{{}, std::move(data)};
}

Result<NullJson> readFramebufferAsPng(ObjectManager &objects, const ImageParams<PngImageParams> &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);

    auto channel = static_cast<FramebufferChannel>(params.settings.channel);
    checkChannelInFramebuffer(framebuffer, channel);

    auto data = readChannelAsPng(framebuffer, params.settings.channel);

    return Result<NullJson>{{}, std::move(data)};
}

Result<NullJson> readFramebufferAsExr(ObjectManager &objects, const ImageParams<ExrImageParams> &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);

    for (auto channel : params.settings.channels)
    {
        checkChannelInFramebuffer(framebuffer, channel);
    }

    auto data = readChannelsAsExr(framebuffer, params.settings.channels);

    return Result<NullJson>{{}, std::move(data)};
}

void addImageEndpoints(ApiBuilder &builder, ObjectManager &objects)
{
    builder.endpoint("readFramebuffer", [&](ImageParams<RawImageParams> params) { return readFramebuffer(objects, params); })
        .description("Read a framebuffer channel and return it in the binary part of the message");

    builder.endpoint("readFramebufferAsJpeg", [&](ImageParams<JpegImageParams> params) { return readFramebufferAsJpeg(objects, params); })
        .description("Read a framebuffer channel and return it encoded as JPEG in the binary part of the message");

    builder.endpoint("readFramebufferAsPng", [&](ImageParams<PngImageParams> params) { return readFramebufferAsPng(objects, params); })
        .description("Read a framebuffer channel and return it encoded as PNG in the binary part of the message");

    builder.endpoint("readFramebufferAsExr", [&](ImageParams<ExrImageParams> params) { return readFramebufferAsExr(objects, params); })
        .description("Read framebuffer channels and return them encoded as EXR in the binary part of the message");
}
}

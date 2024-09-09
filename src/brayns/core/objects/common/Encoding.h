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

#pragma once

#include <brayns/core/codecs/JpegCodec.h>
#include <brayns/core/objects/FramebufferObjects.h>

namespace brayns
{
enum class JpegChannel
{
    Color = int(FramebufferChannel::Color),
    Albedo = int(FramebufferChannel::Albedo),
};

using PngChannel = JpegChannel;

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

std::string readChannel(UserFramebuffer &framebuffer, FramebufferChannel channel);
std::string readChannelAsJpeg(UserFramebuffer &framebuffer, JpegChannel channel, const JpegSettings &settings);
std::string readChannelAsPng(UserFramebuffer &framebuffer, PngChannel channel);
std::string readChannelsAsExr(UserFramebuffer &framebuffer, const std::set<FramebufferChannel> &channels);
}

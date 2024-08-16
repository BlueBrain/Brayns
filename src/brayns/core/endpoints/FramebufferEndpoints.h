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

#include <brayns/core/api/ApiBuilder.h>
#include <brayns/core/engine/Framebuffer.h>
#include <brayns/core/objects/LockedObjects.h>

#include "ImageOperationEndpoints.h"

namespace brayns
{
template<>
struct EnumReflector<FramebufferFormat>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferFormat>();
        builder.field("Rgba", FramebufferFormat::Rgba8).description("8 bit linear RGBA");
        builder.field("Srgba8", FramebufferFormat::Srgba8).description("8 bit gamma-encoded RGB and linear A");
        builder.field("Rgba32F", FramebufferFormat::Rgba32F).description("32 bit float RGBA");
        return builder.build();
    }
};

template<>
struct EnumReflector<FramebufferChannel>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferChannel>();
        builder.field("Color", FramebufferChannel::Color).description("RGBA color as framebuffer format");
        builder.field("Depth", FramebufferChannel::Depth)
            .description("Euclidean distance from camera of the closest hit as 32 bit float");
        builder.field("Normal", FramebufferChannel::Normal).description("Accumulated normal XYZ as 32 bit float");
        builder.field("Albedo", FramebufferChannel::Albedo)
            .description("Accumulated color without illumination RGB as 32 bit float");
        builder.field("PrimitiveId", FramebufferChannel::PrimitiveId)
            .description("Index of first primitive hit as 32 bit int");
        builder.field("ModelId", FramebufferChannel::ModelId)
            .description("ID set by user of the first geometric/volumetric model hit as 32 bit int");
        builder.field("InstanceId", FramebufferChannel::InstanceId)
            .description("ID set by user of the first instance hit as 32 bit int");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Accumulation>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Accumulation>();
        builder.field("variance", [](auto &object) { return &object.variance; })
            .description("Wether to store per-pixel variance in a channel")
            .defaultValue(false);
        return builder.build();
    }
};

struct FramebufferParams
{
    FramebufferSettings settings;
    std::vector<ObjectId> imageOperations;
};

template<>
struct JsonObjectReflector<FramebufferParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferParams>();
        builder.field("resolution", [](auto &object) { return &object.settings.resolution; })
            .description("Framebuffer resolution in pixel");
        builder.field("format", [](auto &object) { return &object.settings.format; })
            .description("Format of the framebuffer color channel")
            .defaultValue(FramebufferFormat::Srgba8);
        builder.field("channels", [](auto &object) { return &object.settings.channels; })
            .description("Framebuffer channels that can be accessed by user")
            .defaultValue(std::set<FramebufferChannel>{FramebufferChannel::Color});
        builder.field("accumulation", [](auto &object) { return &object.settings.accumulation; })
            .description("If not null, the framebuffer will use accumulation with given settings");
        builder.field("imageOperations", [](auto &object) { return &object.imageOperations; })
            .description("List of image operation IDs that will be applied on the framebuffer")
            .defaultValue(std::set<ObjectId>());
        return builder.build();
    }
};

struct UserFramebuffer
{
    Framebuffer deviceObject;
    FramebufferSettings settings;
    std::vector<Stored<ImageOperationInterface>> imageOperations;
};

template<>
struct ObjectReflector<UserFramebuffer>
{
    static std::string getType(const UserFramebuffer &)
    {
        return "Framebuffer";
    }
};

void addFramebufferEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device);
}

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

#include "FramebufferEndpoints.h"

#include <brayns/core/utils/EnumReflector.h>

namespace brayns
{
template<>
struct EnumReflector<FramebufferFormat>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferFormat>();
        builder.field("rgba", FramebufferFormat::Rgba8).description("8 bit linear RGBA");
        builder.field("srgba8", FramebufferFormat::Srgba8).description("8 bit gamma-encoded RGB and linear A");
        builder.field("rgba32F", FramebufferFormat::Rgba32F).description("32 bit float RGBA");
        return builder.build();
    }
};

template<>
struct EnumReflector<FramebufferChannel>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferChannel>();
        builder.field("color", FramebufferChannel::Color).description("RGBA color as framebuffer format");
        builder.field("depth", FramebufferChannel::Depth)
            .description("Euclidean distance from camera of the closest hit as 32 bit float");
        builder.field("normal", FramebufferChannel::Normal).description("Accumulated normal XYZ as 32 bit float");
        builder.field("albedo", FramebufferChannel::Albedo)
            .description("Accumulated color without illumination RGB as 32 bit float");
        builder.field("primitive_id", FramebufferChannel::PrimitiveId)
            .description("Index of first primitive hit as 32 bit int");
        builder.field("model_id", FramebufferChannel::ModelId)
            .description("ID set by user of the first geometric/volumetric model hit as 32 bit int");
        builder.field("instance_id", FramebufferChannel::InstanceId)
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
            .description("Wether to use a framebuffer channel to store pixel variance");
        return builder.build();
    }
};

struct FramebufferParams
{
    Size2 resolution;
    FramebufferFormat format;
    std::set<FramebufferChannel> channels = {FramebufferChannel::Color};
    std::optional<Accumulation> accumulation = std::nullopt;
    std::set<ObjectId> imageOperations = {};
};

struct UserFramebuffer
{
    Framebuffer deviceObject;
    FramebufferSettings settings;
};

void addFramebufferEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    (void)builder;
    (void)objects;
    (void)device;
}
}

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

#include "ImageOperationEndpoints.h"

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
            .description("Wether to store per-pixel variance in a channel");
        return builder.build();
    }
};

struct FramebufferParams
{
    Size2 resolution;
    FramebufferFormat format;
    std::set<FramebufferChannel> channels = {FramebufferChannel::Color};
    std::optional<Accumulation> accumulation = std::nullopt;
    std::vector<ObjectId> imageOperations = {};
};

template<>
struct JsonObjectReflector<FramebufferParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferParams>();
        builder.field("resolution", [](auto &object) { return &object.resolution; })
            .description("Framebuffer resolution in pixel");
        builder.field("format", [](auto &object) { return &object.format; })
            .description("Format of the framebuffer color channel")
            .defaultValue(FramebufferFormat::Srgba8);
        builder.field("channels", [](auto &object) { return &object.channels; })
            .description("Framebuffer channels that can be accessed by user")
            .defaultValue(std::set<FramebufferChannel>{FramebufferChannel::Color});
        builder.field("accumulation", [](auto &object) { return &object.accumulation; })
            .description("If not null, the framebuffer will use accumulation with given settings");
        builder.field("image_operations", [](auto &object) { return &object.imageOperations; })
            .description("List of image operation IDs that will be applied on the framebuffer");
        return builder.build();
    }
};

Data<ImageOperation> getImageOperations(ObjectManager &objects, Device &device, const std::vector<ObjectId> &ids)
{
    auto itemCount = ids.size();

    auto data = allocateData<ImageOperation>(device, itemCount);
    auto items = data.getItems();

    for (auto i = std::size_t(0); i < itemCount; ++i)
    {
        auto id = ids[i];
        auto &interface = objects.get<ImageOperationInterface>(id);

        items[i] = interface.getDeviceObject();
    }

    return data;
}

FramebufferSettings getSettings(ObjectManager &objects, Device &device, const FramebufferParams &params)
{
    return {
        .resolution = params.resolution,
        .format = params.format,
        .channels = params.channels,
        .accumulation = params.accumulation,
        .imageOperations = getImageOperations(objects, device, params.imageOperations),
    };
}

struct UserFramebuffer
{
    Framebuffer deviceObject;
    FramebufferParams params;
};

void addFramebufferEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    (void)builder;
    (void)objects;
    (void)device;
}
}

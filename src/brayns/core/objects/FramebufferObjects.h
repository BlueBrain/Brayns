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

#include <brayns/core/engine/Framebuffer.h>
#include <brayns/core/manager/ObjectManager.h>

#include "ImageOperationObjects.h"

namespace brayns
{
struct UserFramebuffer
{
    std::vector<Stored<UserImageOperation>> operations;
    FramebufferSettings settings;
    Framebuffer value;
};

template<>
struct EnumReflector<FramebufferChannel>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferChannel>();
        builder.field("Color", FramebufferChannel::Color).description("Color RGBA, uint8 or float32 depending on framebuffer format");
        builder.field("Depth", FramebufferChannel::Depth).description("Euclidean distance from camera of the closest hit as float32");
        builder.field("Normal", FramebufferChannel::Normal).description("Accumulated normal XYZ as 3 x float32");
        builder.field("Albedo", FramebufferChannel::Albedo).description("Accumulated color without illumination RGB as 3 x float32");
        builder.field("PrimitiveId", FramebufferChannel::PrimitiveId).description("Index of first primitive hit as uint32");
        builder.field("ModelId", FramebufferChannel::ModelId).description("ID set by user of the first geometric/volumetric model hit as uint32");
        builder.field("InstanceId", FramebufferChannel::InstanceId).description("ID set by user of the first instance hit as uint32");
        return builder.build();
    }
};

template<>
struct EnumReflector<FramebufferFormat>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferFormat>();
        builder.field("Rgba8", FramebufferFormat::Rgba8).description("8 bit linear RGBA");
        builder.field("Srgba8", FramebufferFormat::Srgba8).description("8 bit gamma-encoded RGB and linear A");
        builder.field("Rgba32F", FramebufferFormat::Rgba32F).description("32 bit float RGBA");
        return builder.build();
    }
};

struct FramebufferParams
{
    FramebufferSettings value;
    std::vector<ObjectId> operations;
};

template<>
struct JsonObjectReflector<FramebufferParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferParams>();
        builder.field("resolution", [](auto &object) { return &object.value.resolution; })
            .description("Framebuffer resolution in pixel")
            .defaultValue(Size2(1'920, 1'080))
            .items()
            .minimum(64)
            .maximum(20'000);
        builder.field("format", [](auto &object) { return &object.value.format; })
            .description("Format of the framebuffer color channel")
            .defaultValue(FramebufferFormat::Srgba8);
        builder.field("channels", [](auto &object) { return &object.value.channels; })
            .description("Framebuffer channels that can be accessed by user")
            .defaultValue(std::set<FramebufferChannel>{FramebufferChannel::Color});
        builder.field("accumulation", [](auto &object) { return &object.value.accumulation; })
            .description("Wether to use an accumulation channel (progressive rendering)")
            .defaultValue(false);
        builder.field("variance", [](auto &object) { return &object.value.variance; })
            .description("Wether to use a variance channel if accumulation is enabled (to have an estimate)")
            .defaultValue(false);
        builder.field("operations", [](auto &object) { return &object.operations; })
            .description("List of image operation IDs that will be applied on the framebuffer")
            .defaultValue(std::vector<ObjectId>());
        return builder.build();
    }
};

struct FramebufferResult
{
    FramebufferParams settings;
    std::optional<float> variance;
};

template<>
struct JsonObjectReflector<FramebufferResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferResult>();
        builder.extend([](auto &object) { return &object.settings; });
        builder.field("varianceEstimate", [](auto &object) { return &object.variance; })
            .description("Variance of the framebuffer (null if no variance channels or nothing has been rendered yet)");
        return builder.build();
    }
};

using CreateFramebufferParams = CreateParamsOf<FramebufferParams>;
using GetFramebufferResult = GetResultOf<FramebufferResult>;

CreateObjectResult createFramebuffer(ObjectManager &objects, Device &device, const CreateFramebufferParams &params);
GetFramebufferResult getFramebuffer(ObjectManager &objects, const GetObjectParams &params);
void clearFramebuffer(ObjectManager &objects, const GetObjectParams &params);
}

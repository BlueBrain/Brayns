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
#include <brayns/core/manager/LockedObjects.h>

#include "ImageOperationEndpoints.h"

namespace brayns
{
template<>
struct EnumReflector<FramebufferChannel>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferChannel>();
        builder.field("Color", FramebufferChannel::Color)
            .description("Color RGBA, uint8 or float32 depending on framebuffer format");
        builder.field("Depth", FramebufferChannel::Depth)
            .description("Euclidean distance from camera of the closest hit as float32");
        builder.field("Normal", FramebufferChannel::Normal).description("Accumulated normal XYZ as 3 x float32");
        builder.field("Albedo", FramebufferChannel::Albedo)
            .description("Accumulated color without illumination RGB as 3 x float32");
        builder.field("PrimitiveId", FramebufferChannel::PrimitiveId)
            .description("Index of first primitive hit as uint32");
        builder.field("ModelId", FramebufferChannel::ModelId)
            .description("ID set by user of the first geometric/volumetric model hit as uint32");
        builder.field("InstanceId", FramebufferChannel::InstanceId)
            .description("ID set by user of the first instance hit as uint32");
        return builder.build();
    }
};

struct UserFramebuffer
{
    Framebuffer deviceObject;
    FramebufferSettings settings;
    std::vector<Stored<ImageOperationInterface>> imageOperations;
};

void addFramebufferEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device);
}

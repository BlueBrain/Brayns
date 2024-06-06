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

#include "Data.h"
#include "Object.h"

namespace brayns::experimental
{
class ImageOperation : public Managed<OSPImageOperation>
{
public:
    using Managed::Managed;
};

struct ToneMapperSettings
{
    float exposure = 1.0F;
    float contrast = 1.6773F;
    float hightlightCompression = 0.9714F;
    float midLevelAnchorInput = 0.18F;
    float midLevelAnchorOutput = 0.18F;
    float maxHdr = 11.0785F;
    bool aces = true;
};

class ToneMapper : public ImageOperation
{
public:
    using ImageOperation::ImageOperation;
};

void loadToneMapperParams(OSPImageOperation handle, const ToneMapperSettings &settings);

enum class FramebufferFormat
{
    Rgba8 = OSP_FB_RGBA8,
    Srgba8 = OSP_FB_SRGBA,
    Rgba32F = OSP_FB_RGBA32F,
};

enum class FramebufferChannel
{
    Color = OSP_FB_COLOR,
    Depth = OSP_FB_DEPTH,
    Accumulation = OSP_FB_ACCUM,
    Variance = OSP_FB_VARIANCE,
    Normal = OSP_FB_NORMAL,
    Albedo = OSP_FB_ALBEDO,
    PrimitiveId = OSP_FB_ID_PRIMITIVE,
    ModelId = OSP_FB_ID_OBJECT,
    InstanceId = OSP_FB_ID_INSTANCE,
};

FramebufferChannel operator|(FramebufferChannel left, FramebufferChannel right)
{
    return static_cast<FramebufferChannel>(static_cast<int>(left) | static_cast<int>(right));
}

FramebufferChannel &operator|=(FramebufferChannel &left, FramebufferChannel right)
{
    return left = left | right;
}

bool operator&(FramebufferChannel channels, FramebufferChannel channel)
{
    return static_cast<int>(channels) & static_cast<int>(channel) != 0;
}

struct FramebufferSettings
{
    std::size_t width;
    std::size_t height;
    FramebufferFormat format = FramebufferFormat::Srgba8;
    FramebufferChannel channels = FramebufferChannel::Color;
    std::span<ImageOperation> operations = {};
};

void loadFramebufferParams(OSPFrameBuffer handle, const FramebufferSettings &settings);

class Framebuffer : public Managed<OSPFrameBuffer>
{
public:
    using Managed::Managed;

    const void *map(FramebufferChannel channel);
    void unmap(const void *data);
    void resetAccumulation();
    float getVariance();
};
}

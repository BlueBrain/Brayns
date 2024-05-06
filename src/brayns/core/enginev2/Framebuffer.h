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
#include "Managed.h"

namespace brayns::experimental
{
class ImageOperation : public Managed<OSPImageOperation>
{
public:
    using Managed::Managed;
};

class ToneMapper : public ImageOperation
{
public:
    static inline const std::string name = "tonemapper";

    using ImageOperation::ImageOperation;
};

enum class FramebufferFormat
{
    Rgba8 = OSP_FB_RGBA8,
    Srgba8 = OSP_FB_SRGBA,
    Rgba32f = OSP_FB_RGBA32F,
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

struct FramebufferSettings
{
    std::size_t width;
    std::size_t height;
    FramebufferFormat format = FramebufferFormat::Srgba8;
    std::vector<FramebufferChannel> channels = {FramebufferChannel::Color};
};

class Framebuffer : public Managed<OSPFrameBuffer>
{
public:
    using Managed::Managed;

    const void *map(FramebufferChannel channel);
    void unmap(const void *data);
    void resetAccumulation();
    float getVariance();
    void setImageOperations(SharedArray<ImageOperation> operations);
};
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::ImageOperation, OSP_IMAGE_OPERATION)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::ToneMapper, OSP_IMAGE_OPERATION)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Framebuffer, OSP_FRAMEBUFFER)
}

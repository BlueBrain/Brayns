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

#include <memory>
#include <optional>
#include <set>

#include "Data.h"
#include "Device.h"
#include "ImageOperation.h"
#include "Object.h"

namespace brayns
{
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
    Normal = OSP_FB_NORMAL,
    Albedo = OSP_FB_ALBEDO,
    PrimitiveId = OSP_FB_ID_PRIMITIVE,
    ModelId = OSP_FB_ID_OBJECT,
    InstanceId = OSP_FB_ID_INSTANCE,
};

struct Accumulation
{
    bool variance = true;
};

struct FramebufferSettings
{
    Size2 resolution;
    FramebufferFormat format = FramebufferFormat::Srgba8;
    std::set<FramebufferChannel> channels = {FramebufferChannel::Color};
    std::optional<Accumulation> accumulation = std::nullopt;
    std::optional<Data<ImageOperation>> imageOperations = std::nullopt;
};

class FramebufferData
{
public:
    explicit FramebufferData(const void *data, OSPFrameBuffer handle);

    const void *get() const;

    template<typename T>
    const T *as() const
    {
        return static_cast<const T *>(get());
    }

private:
    struct Deleter
    {
        OSPFrameBuffer handle;

        void operator()(const void *data) const;
    };

    std::unique_ptr<const void, Deleter> _data;
};

class Framebuffer : public Managed<OSPFrameBuffer>
{
public:
    using Managed::Managed;

    FramebufferData map(FramebufferChannel channel);
    void resetAccumulation();
    std::optional<float> getVariance();
    void setImageOperations(const std::optional<Data<ImageOperation>> &imageOperations);
};

Framebuffer createFramebuffer(Device &device, const FramebufferSettings &settings);
}

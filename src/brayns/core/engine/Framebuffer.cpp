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

#include "Framebuffer.h"

namespace brayns
{
const void *Framebuffer::map(FramebufferChannel channel)
{
    auto handle = getHandle();
    return ospMapFrameBuffer(handle, static_cast<OSPFrameBufferChannel>(channel));
}

void Framebuffer::unmap(const void *data)
{
    auto handle = getHandle();
    ospUnmapFrameBuffer(data, handle);
}

void Framebuffer::resetAccumulation()
{
    auto handle = getHandle();
    ospResetAccumulation(handle);
}

float Framebuffer::getVariance()
{
    auto handle = getHandle();
    return ospGetVariance(handle);
}

Framebuffer createFramebuffer(Device &device, const FramebufferSettings &settings)
{
    auto width = static_cast<int>(settings.resolution[0]);
    auto height = static_cast<int>(settings.resolution[1]);
    auto format = static_cast<OSPFrameBufferFormat>(settings.format);
    auto channels = static_cast<std::uint32_t>(OSP_FB_NONE);
    auto accumulation = settings.accumulation.has_value();
    auto variance = settings.accumulation && settings.accumulation->variance;

    for (auto channel : settings.channels)
    {
        channels |= static_cast<OSPFrameBufferChannel>(channel);
    }

    if (accumulation)
    {
        channels |= OSP_FB_ACCUM;
    }

    if (variance)
    {
        channels |= OSP_FB_VARIANCE;
    }

    auto handle = ospNewFrameBuffer(width, height, format, channels);
    auto framebuffer = wrapObjectHandleAs<Framebuffer>(device, handle);

    setObjectParam(handle, "imageOperation", settings.imageOperations);

    commitObject(handle);

    return framebuffer;
}
}

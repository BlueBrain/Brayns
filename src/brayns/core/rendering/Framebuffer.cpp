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
const void *brayns::FrameBuffer::map(Channel channel)
{
    return getHandle().map(static_cast<OSPFrameBufferChannel>(channel));
}

void FrameBuffer::unmap(const void *data)
{
    return getHandle().unmap(const_cast<void *>(data));
}

void FrameBuffer::resetAccumulation()
{
    getHandle().resetAccumulation();
}

float FrameBuffer::getVariance()
{
    return getHandle().variance();
}

void FrameBuffer::setImageOperations(CopiedArray<ImageOperation> operations)
{
    auto handles = extractHandles(operations);
    setParam("imageOperation", ospray::cpp::CopiedData(handles));
}
}

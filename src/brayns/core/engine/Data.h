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

#include "Device.h"
#include "Object.h"

#include <vector>

namespace brayns
{
class Data : public Managed<OSPData>
{
public:
    using Managed::Managed;
};

inline Data createData(const void *ptr, DataType type, const Size3 &size, const Stride3 &stride = {0, 0, 0})
{
    auto *handle = ospNewSharedData(ptr, type, size.x, stride.x, size.y, stride.y, size.z, stride.z);

    if (handle == nullptr)
    {
        throw DeviceException(OSP_UNKNOWN_ERROR, "Failed to allocate OSPRay data wrapper");
    }

    return Data(handle);
}

template<OsprayDataType T>
struct ObjectParamReflector<std::vector<T>>
{
    static void set(OSPObject handle, const char *id, const std::vector<T> &items)
    {
        if (items.empty())
        {
            removeObjectParam(handle, id);
            return;
        }

        auto data = createData(items.data(), dataTypeOf<T>, {items.size(), 1, 1});

        setObjectParam(handle, id, data);
    }
};
}

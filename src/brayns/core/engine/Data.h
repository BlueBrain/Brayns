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

#include <span>

#include "Object.h"

namespace brayns
{
class Data : public Managed<OSPData>
{
public:
    using Managed::Managed;
};

template<OsprayDataType T>
Data toSharedData(std::span<T> items)
{
    auto data = items.data();
    auto type = ospray::OSPTypeFor<T>::value;
    auto size = items.size();
    auto handle = ospNewSharedData(data, type, size);
    return Data(handle);
}

template<OsprayDataType T>
void setObjectData(OSPObject handle, const char *id, std::span<T> items)
{
    setObjectParam(handle, id, toSharedData(items));
}

template<OsprayDataType T>
void setObjectDataIfNotEmpty(OSPObject handle, const char *id, std::span<T> items)
{
    if (items.empty())
    {
        removeObjectParam(handle, id);
        return;
    }
    setObjectData(handle, id, items);
}
}
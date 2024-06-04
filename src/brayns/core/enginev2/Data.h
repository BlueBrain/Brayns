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

namespace brayns::experimental
{
using DataType = OSPDataType;

struct DataSettings
{
    DataType type;
    Size3 itemCount;
    const void *data;
    void (*deleter)(const void *, const void *) = nullptr;
};

class Data : public Managed<OSPData>
{
public:
    using Managed::Managed;
};

template<typename ItemType, std::size_t DimensionCount>
class DataND : public Data
{
public:
    using Data::Data;
};

template<typename ItemType>
using Data1D = DataND<ItemType, 1>;

template<typename ItemType>
using Data2D = DataND<ItemType, 2>;

template<typename ItemType>
using Data3D = DataND<ItemType, 3>;
}

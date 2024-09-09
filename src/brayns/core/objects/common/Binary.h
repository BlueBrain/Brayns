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

#include <ranges>
#include <span>
#include <utility>

#include <fmt/format.h>

#include <brayns/core/engine/Data.h>
#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/utils/Binary.h>

namespace brayns
{
template<typename T>
std::size_t getItemCountFromBinaryOf(std::string_view bytes)
{
    auto bytesSize = bytes.size();
    auto itemSize = getBinarySize<T>();
    auto itemCount = bytesSize / itemSize;

    if (itemCount * itemSize != bytesSize)
    {
        throw InvalidParams(fmt::format("Binary size {} is not a multiple of item size {}", bytesSize, itemSize));
    }

    return itemCount;
}

template<typename T>
void copyBinaryTo(std::string_view bytes, std::span<T> items)
{
    for (auto &item : items)
    {
        extractBytesTo(bytes, std::endian::little, item);
    }
}

template<typename T>
Data3D<T> createData3DFromBinaryOf(Device &device, const Size3 &itemCount, std::string_view bytes)
{
    auto totalItemCount = getItemCountFromBinaryOf<T>(bytes);

    if (totalItemCount == 0)
    {
        throw InvalidParams("Empty data is not supported by OSPRay");
    }

    if (reduceMultiply(itemCount) != totalItemCount)
    {
        throw InvalidParams(
            fmt::format(
                "Item count in binary {} does not match given item count {}x{}x{}",
                totalItemCount,
                itemCount.x,
                itemCount.y,
                itemCount.z));
    }

    auto data = allocateData3D<T>(device, itemCount);

    copyBinaryTo(bytes, data.getItems());

    return data;
}

template<typename T>
Data2D<T> createData2DFromBinaryOf(Device &device, const Size2 &itemCount, std::string_view bytes)
{
    auto size3 = Size3(itemCount, 1);
    auto data3 = createData3DFromBinaryOf<T>(device, size3, bytes);
    return Data2D<T>(std::move(data3));
}

template<typename T>
Data<T> createDataFromBinaryOf(Device &device, std::size_t itemCount, std::string_view bytes)
{
    auto size3 = Size3(itemCount, 1, 1);
    auto data3 = createData3DFromBinaryOf<T>(device, size3, bytes);
    return Data<T>(std::move(data3));
}

template<std::ranges::range T>
std::string composeRangeToBinary(T items)
{
    auto output = std::string();
    output.reserve(items.size() + getBinarySize<std::ranges::range_value_t<T>>());

    for (const auto &item : items)
    {
        composeBytesTo(item, std::endian::little, output);
    }

    return output;
}
}

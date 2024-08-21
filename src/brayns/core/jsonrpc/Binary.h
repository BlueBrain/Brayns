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
#include <utility>

#include <brayns/core/json/Json.h>
#include <brayns/core/utils/Binary.h>

#include "Errors.h"

namespace brayns
{
template<typename T>
std::vector<T> extractBytesAsVectorOf(std::string_view &bytes, std::size_t itemCount)
{
    if (bytes.size() < itemCount * getBinarySize<T>())
    {
        throw InvalidParams("Binary part of the message is too small");
    }

    auto items = std::vector<T>();
    items.resize(itemCount);

    for (auto &item : items)
    {
        extractBytesTo(bytes, std::endian::little, item);
    }

    return items;
}

template<typename T>
std::vector<T> parseBytesAsVectorOf(std::string_view bytes, std::size_t itemCount)
{
    return extractBytesAsVectorOf<T>(bytes, itemCount);
}

template<typename T>
std::vector<T> parseBytesAsVectorOf(std::string_view bytes)
{
    auto size = bytes.size();
    auto itemSize = getBinarySize<T>();
    auto itemCount = size / itemSize;

    if (size != itemCount * itemSize)
    {
        throw InvalidParams("Binary part of the message must be a multiple of the item size");
    }

    return extractBytesAsVectorOf<T>(bytes, itemCount);
}

template<std::ranges::range T>
std::string composeRangeToBinary(T items)
{
    auto output = std::string();
    output.reserve(items.size() + getBinarySize<std::ranges::range_value_t<T>>());

    auto endian = std::endian::little;

    for (const auto &item : items)
    {
        composeBytesTo(item, endian, output);
    }

    return output;
}
}

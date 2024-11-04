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

#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/utils/Binary.h>

namespace brayns
{
inline void sanitizeBinary(std::span<char> bytes, std::size_t itemSize)
{
    auto bytesSize = bytes.size();
    auto itemCount = bytesSize / itemSize;

    if (itemCount * itemSize != bytesSize)
    {
        throw InvalidParams(fmt::format("Binary size {} is not a multiple of item size {}", bytesSize, itemSize));
    }

    if constexpr (std::endian::native != std::endian::little)
    {
        for (auto i = std::size_t(0); i < itemCount; ++i)
        {
            auto item = bytes.subspan(i * itemSize, (i + 1) * itemSize);
            std::ranges::reverse(item);
        }
    }
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

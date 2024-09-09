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

#include <cassert>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include <brayns/core/utils/Math.h>

namespace brayns
{
enum class ImageFormat
{
    Rgb8,
    Rgba8,
};

enum class RowOrder
{
    TopDown,
    BottomUp,
};

struct ImageView
{
    const void *data;
    Size2 size;
    ImageFormat format;
    RowOrder rowOrder = RowOrder::BottomUp;
};

inline char encodePixelTo8Bit(float value)
{
    return static_cast<char>(std::round(value * 255.0F));
}

inline float encodePixelToFloat(char value)
{
    return static_cast<float>(value) / 255.0F;
}

template<typename T, int S>
std::string convertTo8Bit(std::span<const Vector<T, S>> items, std::size_t pixelSize = S)
{
    auto data = std::string();
    data.reserve(pixelSize * items.size());

    for (const auto &item : items)
    {
        for (auto i = std::size_t(0); i < pixelSize; ++i)
        {
            data.push_back(encodePixelTo8Bit(item[i]));
        }
    }

    return data;
}

template<int S>
std::vector<Vector<float, S>> convertToFloat(std::string_view data)
{
    constexpr auto pixelSize = std::size_t(S);

    auto itemCount = data.size() / pixelSize;

    assert(data.size() % pixelSize == 0);

    auto items = std::vector<Vector<float, S>>();
    items.reserve(itemCount);

    for (auto i = std::size_t(0); i < itemCount; ++i)
    {
        auto &item = items.emplace_back();

        for (auto j = std::size_t(0); j < pixelSize; ++j)
        {
            auto index = i * pixelSize + j;

            item[j] = encodePixelToFloat(data[index]);
        }
    }

    return items;
}
}

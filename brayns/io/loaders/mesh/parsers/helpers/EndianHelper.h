/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <cstdint>

namespace brayns
{
class EndianHelper
{
public:
    static bool isBigEndian()
    {
        int32_t test = 1;
        return *reinterpret_cast<char *>(&test) == 0;
    }

    template <typename T>
    static T convertLittleEndianToLocalEndian(T value)
    {
        return isBigEndian() ? swapBytes(value) : value;
    }

    template <typename T>
    static T convertBigEndianToLocalEndian(T value)
    {
        return isBigEndian() ? value : swapBytes(value);
    }

    template <typename T>
    static T swapBytes(T value)
    {
        T copy;
        auto from = reinterpret_cast<const char *>(&value);
        auto to = reinterpret_cast<char *>(&copy);
        auto stride = sizeof(T);
        for (size_t i = 0; i < stride; ++i)
        {
            to[i] = from[stride - i - 1];
        }
        return copy;
    }
};
} // namespace brayns

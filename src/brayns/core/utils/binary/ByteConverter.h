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

#include <bit>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

namespace brayns
{
class ByteConverter
{
public:
    static_assert(
        std::endian::native == std::endian::big || std::endian::native == std::endian::little,
        "Unsupported native endian");

    template<typename T>
    static const char *getBytes(const T &value)
    {
        return reinterpret_cast<const char *>(&value);
    }

    template<typename T>
    static char *getBytes(T &value)
    {
        return reinterpret_cast<char *>(&value);
    }

    template<typename T>
    static T swapBytes(T value)
    {
        auto bytes = getBytes(value);
        auto stride = sizeof(T);
        for (size_t i = 0; i < stride / 2; ++i)
        {
            std::swap(bytes[i], bytes[stride - 1 - i]);
        }
        return value;
    }

    template<typename T>
    static T convertFromLocalEndian(T value, std::endian endian)
    {
        if (std::endian::native == endian)
        {
            return value;
        }
        return swapBytes(value);
    }

    template<typename T>
    static T convertToLocalEndian(T value, std::endian endian)
    {
        if (std::endian::native == endian)
        {
            return value;
        }
        return swapBytes(value);
    }

    template<typename T>
    static T convertFromBytes(std::string_view data, std::endian endian)
    {
        auto stride = sizeof(T);
        if (data.size() != stride)
        {
            throw std::invalid_argument("Invalid byte count");
        }
        auto value = T();
        std::memcpy(&value, data.data(), stride);
        return convertToLocalEndian(value, endian);
    }

    template<typename T>
    static std::string convertToBytes(T value, std::endian endian)
    {
        value = convertFromLocalEndian(value, endian);
        auto stride = sizeof(T);
        auto bytes = getBytes(value);
        return {bytes, stride};
    }
};
} // namespace brayns

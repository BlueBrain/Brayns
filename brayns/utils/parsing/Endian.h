/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "ByteConverter.h"

namespace brayns
{
class NativeEndian
{
public:
    static constexpr uint32_t test = 0x01020304;

    static bool isBigEndian()
    {
    }
};

enum class Endian
{
    Little = 0,
    Big = 1,
    Local = EndianHelper::isBigEndian() ? Big : Little
};

class EndianConverter
{
public:
    static void convertToLocalEndian(char *bytes, size_t stride, Endian endian)
    {
        if (endian != Endian::Local)
        {
            ByteConverter::swapBytes(bytes, stride);
        }
    }

    static void convertFromLocalEndian(char *bytes, size_t stride, Endian endian)
    {
        convertToLocalEndian(bytes, stride, endian);
    }

    template<typename T>
    static void convertToLocalEndian(T &value, Endian endian)
    {
        auto bytes = ByteConverter::getBytes(value);
        convertToLocalEndian(bytes, sizeof(T), endian);
    }

    template<typename T>
    static void convertFromLocalEndian(T &value, Endian endian)
    {
        convertToLocalEndian(value, endian);
    }
};
} // namespace brayns

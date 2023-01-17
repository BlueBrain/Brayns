/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include "ByteConverter.h"

namespace brayns
{
enum class ByteOrder
{
    BigEndian,
    LittleEndian
};

class ByteOrderHelper
{
public:
    static ByteOrder getSystemByteOrder();
    static void convertToSystemByteOrder(char *bytes, size_t stride, ByteOrder order);
    static void convertFromSystemByteOrder(char *bytes, size_t stride, ByteOrder order);

    template<typename T>
    static void convertToSystemByteOrder(T &value, ByteOrder order)
    {
        auto bytes = ByteConverter::getBytes(value);
        convertToSystemByteOrder(bytes, sizeof(T), order);
    }

    template<typename T>
    static void convertFromSystemByteOrder(T &value, ByteOrder order)
    {
        auto bytes = ByteConverter::getBytes(value);
        convertFromSystemByteOrder(bytes, sizeof(T), order);
    }
};
} // namespace brayns

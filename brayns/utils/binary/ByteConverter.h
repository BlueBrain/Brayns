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

#include <cstddef>

namespace brayns
{
class ByteConverter
{
public:
    static void copyBytes(const char *from, char *to, size_t stride);
    static void swapBytes(char *bytes, size_t stride);

    template<typename T>
    static const char *getBytes(const T &value)
    {
        auto address = static_cast<const void *>(&value);
        return static_cast<const char *>(address);
    }

    template<typename T>
    static char *getBytes(T &value)
    {
        auto address = static_cast<void *>(&value);
        return static_cast<char *>(address);
    }

    template<typename T>
    static void copyBytes(const T &from, T &to)
    {
        auto source = getBytes(from);
        auto destination = getBytes(to);
        return copyBytes(source, destination, sizeof(T));
    }

    template<typename T>
    static void swapBytes(T &value)
    {
        auto bytes = getBytes(value);
        swapBytes(bytes, sizeof(T));
    }
};
} // namespace brayns

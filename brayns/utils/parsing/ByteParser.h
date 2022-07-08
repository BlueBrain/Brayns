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

#include <array>
#include <vector>

#include "ByteConverter.h"
#include "Endian.h"
#include "StringStream.h"

namespace brayns
{
class ByteParserHelper
{
public:
    static void copyBytes(StringStream &stream, char *bytes, size_t stride);
};

template<typename T>
struct ByteParser
{
    static void parse(StringStream &stream, T &value, Endian endian)
    {
        auto bytes = ByteConverter::getBytes(value);
        ByteParserHelper::copyBytes(stream, bytes, sizeof(T));
        EndianConverter::convertToLocalEndian(value, endian);
    }
};

template<typename T>
struct ByteParser<std::vector<T>>
{
    static void parse(StringStream &stream, std::vector<T> &values, Endian endian)
    {
        while (!stream.isSpace())
        {
            auto &value = values.emplace_back();
            ByteParser<T>::parse(stream, value, endian);
        }
    }
};

template<typename T, size_t S>
struct ByteParser<std::array<T, S>>
{
    static void parse(StringStream &stream, std::array<T, S> &values, Endian endian)
    {
        for (auto &value : values)
        {
            ByteParser<T>::parse(stream, value, endian);
        }
    }
};
} // namespace brayns

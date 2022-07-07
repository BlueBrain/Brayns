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
#include <string_view>
#include <vector>

#include "MathTypes.h"

#include <brayns/utils/binary/ByteParser.h>
#include <brayns/utils/string/StringStream.h>

namespace brayns
{
class BinaryParser
{
public:
    template<typename T>
    static T parse(std::string_view data, Endian endian)
    {
        auto stream = StringStream(data);
        return parse<T>(stream, endian);
    }

    template<typename T>
    static void parse(std::string_view data, Endian endian, T &value)
    {
        auto stream = StringStream(data);
        parse(stream, endian, value);
    }

    template<typename T>
    static T parse(StringStream &stream, Endian endian)
    {
        T value{};
        parse(stream, endian, value);
        return value;
    }

    template<typename T>
    static void parse(StringStream &stream, Endian endian, T &value)
    {
        auto data = stream.extract(sizeof(T));
        ByteParser<T>::parse(data, endian, value);
    }

    template<glm::length_t S, typename T>
    static void parse(StringStream &stream, Endian endian, glm::vec<S, T> &value)
    {
        for (glm::length_t i = 0; i < S; ++i)
        {
            parse(stream, endian, value[i]);
        }
    }

    template<typename T, size_t S>
    static void parse(StringStream &stream, Endian endian, std::array<T, S> &value)
    {
        for (size_t i = 0; i < S; ++i)
        {
            parse(stream, endian, value[i]);
        }
    }

    template<typename T>
    static void parse(StringStream &stream, Endian endian, std::vector<T> &value)
    {
        for (auto &item : value)
        {
            parse(stream, endian, item);
        }
    }
};
} // namespace brayns

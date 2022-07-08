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

#include "ByteParser.h"
#include "StringParser.h"
#include "TokenParser.h"

namespace brayns
{
class Parse
{
public:
    template<typename T>
    static T fromString(std::string_view data)
    {
        T value{};
        fromString(data, value);
        return value;
    }

    template<typename T>
    static void fromString(std::string_view data, T &value)
    {
        StringParser<T>::parse(data, value);
    }

    template<typename T>
    static T fromTokens(std::string_view data)
    {
        auto stream = StringStream(data);
        return fromTokens<T>(stream);
    }

    template<typename T>
    static void fromTokens(std::string_view data, T &value)
    {
        auto stream = StringStream(data);
        fromTokens(stream, value);
    }

    template<typename T>
    static T fromTokens(StringStream &stream)
    {
        T value{};
        fromTokens(stream, value);
        return value;
    }

    template<typename T>
    static void fromTokens(StringStream &stream, T &value)
    {
        return TokenParser<T>::parse(stream, value);
    }

    template<typename T>
    static T fromBytes(std::string_view data, Endian endian)
    {
        auto stream = StringStream(data);
        return fromBytes<T>(stream, endian);
    }

    template<typename T>
    static void fromBytes(std::string_view data, Endian endian, T &value)
    {
        auto stream = StringStream(data);
        fromBytes(stream, endian, value);
    }

    template<typename T>
    static T fromBytes(StringStream &stream, Endian endian)
    {
        T value{};
        fromBytes(stream, endian, value);
        return value;
    }

    template<typename T>
    static void fromBytes(StringStream &stream, Endian endian, T &value)
    {
        ByteParser<T>::parse(stream, endian, value);
    }
};
} // namespace brayns

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

#include <string_view>

#include "binary/ByteParser.h"
#include "string/StringParser.h"

namespace brayns
{
class Convert
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
    static T fromLocalEndian(std::string_view data)
    {
        T value{};
        fromLocalEndian(data, value);
        return value;
    }

    template<typename T>
    static void fromLocalEndian(std::string_view data, T &value)
    {
        ByteParser<T>::parseLocalEndian(data, value);
    }

    template<typename T>
    static T fromLittleEndian(std::string_view data)
    {
        T value{};
        fromLittleEndian(data, value);
        return value;
    }

    template<typename T>
    static void fromLittleEndian(std::string_view data, T &value)
    {
        ByteParser<T>::parseLittleEndian(data, value);
    }

    template<typename T>
    static T fromBigEndian(std::string_view data)
    {
        T value{};
        fromBigEndian(data, value);
        return value;
    }

    template<typename T>
    static void fromBigEndian(std::string_view data, T &value)
    {
        ByteParser<T>::parseBigEndian(data, value);
    }
};
} // namespace brayns

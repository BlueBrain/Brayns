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

#include <brayns/utils/binary/ByteParser.h>
#include <brayns/utils/string/StringParser.h>

#include "ChunkExtractor.h"
#include "RangeParser.h"
#include "TokenExtractor.h"

namespace brayns
{
class Parser
{
public:
    template<typename T>
    static T parseString(std::string_view data)
    {
        T value{};
        parseString(data, value);
        return value;
    }

    template<typename T>
    static void parseString(std::string_view data, T &value)
    {
        StringParser<T>::parse(data, value);
    }

    template<typename T>
    static T extractToken(std::string_view &data)
    {
        T value{};
        extractToken(data, value);
        return value;
    }

    template<typename T>
    static void extractToken(std::string_view &data, T &value)
    {
        TokenExtractor<T>::extract(data, value);
    }

    template<typename T>
    static T parseBytes(std::string_view data, std::endian endian)
    {
        T value{};
        parseBytes(data, value, endian);
        return value;
    }

    template<typename T>
    static void parseBytes(std::string_view data, T &value, std::endian endian)
    {
        ByteParser<T>::parse(data, value, endian);
    }

    template<typename T>
    static T extractChunk(std::string_view &data, std::endian endian)
    {
        T value{};
        extractChunk(data, value, endian);
        return value;
    }

    template<typename T>
    static void extractChunk(std::string_view &data, T &value, std::endian endian)
    {
        ChunkExtractor<T>::extract(data, value, endian);
    }

    template<typename T>
    static std::vector<T> parseRange(std::string_view data)
    {
        return RangeParser<T>::parse(data);
    }
};
} // namespace brayns

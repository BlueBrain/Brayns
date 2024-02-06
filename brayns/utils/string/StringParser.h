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

#include <string_view>

namespace brayns
{
template<typename T>
struct StringParser
{
    static void parse(std::string_view data, T &value)
    {
        value = T(data);
    }
};

template<>
struct StringParser<bool>
{
    static void parse(std::string_view data, bool &value);
};

template<>
struct StringParser<char>
{
    static void parse(std::string_view data, char &value);
};

template<>
struct StringParser<int8_t>
{
    static void parse(std::string_view data, int8_t &value);
};

template<>
struct StringParser<uint8_t>
{
    static void parse(std::string_view data, uint8_t &value);
};

template<>
struct StringParser<int16_t>
{
    static void parse(std::string_view data, int16_t &value);
};

template<>
struct StringParser<uint16_t>
{
    static void parse(std::string_view data, uint16_t &value);
};

template<>
struct StringParser<int32_t>
{
    static void parse(std::string_view data, int32_t &value);
};

template<>
struct StringParser<uint32_t>
{
    static void parse(std::string_view data, uint32_t &value);
};

template<>
struct StringParser<int64_t>
{
    static void parse(std::string_view data, int64_t &value);
};

template<>
struct StringParser<uint64_t>
{
    static void parse(std::string_view data, uint64_t &value);
};

template<>
struct StringParser<float>
{
    static void parse(std::string_view data, float &value);
};

template<>
struct StringParser<double>
{
    static void parse(std::string_view data, double &value);
};
} // namespace brayns

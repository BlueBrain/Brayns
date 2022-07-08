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
struct StringParser<unsigned char>
{
    static void parse(std::string_view data, unsigned char &value);
};

template<>
struct StringParser<short>
{
    static void parse(std::string_view data, short &value);
};

template<>
struct StringParser<unsigned short>
{
    static void parse(std::string_view data, unsigned short &value);
};

template<>
struct StringParser<int>
{
    static void parse(std::string_view data, int &value);
};

template<>
struct StringParser<unsigned int>
{
    static void parse(std::string_view data, unsigned int &value);
};

template<>
struct StringParser<long>
{
    static void parse(std::string_view data, long &value);
};

template<>
struct StringParser<unsigned long>
{
    static void parse(std::string_view data, unsigned long &value);
};

template<>
struct StringParser<long long>
{
    static void parse(std::string_view data, long long &value);
};

template<>
struct StringParser<unsigned long long>
{
    static void parse(std::string_view data, unsigned long long &value);
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

template<>
struct StringParser<long double>
{
    static void parse(std::string_view data, long double &value);
};
} // namespace brayns

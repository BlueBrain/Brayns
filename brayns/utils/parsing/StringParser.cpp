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

#include "StringParser.h"

#include <stdexcept>
#include <string>
#include <type_traits>

namespace
{
template<typename T>
struct BufferType
{
    static_assert(std::is_integral_v<T> && sizeof(T) <= sizeof(long));

    using Type = std::conditional_t<std::is_unsigned_v<T>, unsigned long, long>;
};

template<typename T>
using GetBufferType = typename BufferType<T>::Type;

class IntegerStringParser
{
public:
    template<typename T>
    static void parse(std::string_view data, T &value)
    {
        using Buffer = GetBufferType<T>;
        Buffer buffer;
        brayns::StringParser<Buffer>::parse(data, buffer);
        auto min = std::numeric_limits<T>::min();
        auto max = std::numeric_limits<T>::max();
        if (buffer < min)
        {
            throw std::out_of_range("Value " + std::to_string(buffer) + " < " + std::to_string(min));
        }
        if (buffer > max)
        {
            throw std::out_of_range("Value " + std::to_string(buffer) + " < " + std::to_string(max));
        }
        return static_cast<T>(buffer);
    }
};
} // namespace

namespace brayns
{
void StringParser<bool>::parse(std::string_view data, bool &value)
{
    if (data == "1" || data == "true")
    {
        value = true;
        return;
    }
    if (data == "0" || data == "false")
    {
        value = false;
        return;
    }
    throw std::invalid_argument("Cannot parse boolean from '" + std::string(data) + "'");
}

void StringParser<char>::parse(std::string_view data, char &value)
{
    IntegerStringParser::parse(data, value);
}

void StringParser<unsigned char>::parse(std::string_view data, unsigned char &value)
{
    IntegerStringParser::parse(data, value);
}

void StringParser<short>::parse(std::string_view data, short &value)
{
    IntegerStringParser::parse(data, value);
}

void StringParser<unsigned short>::parse(std::string_view data, unsigned short &value)
{
    IntegerStringParser::parse(data, value);
}

void StringParser<int>::parse(std::string_view data, int &value)
{
    value = std::stoi(std::string(data));
}

void StringParser<unsigned int>::parse(std::string_view data, unsigned int &value)
{
    IntegerStringParser::parse(data, value);
}

void StringParser<long>::parse(std::string_view data, long &value)
{
    value = std::stol(std::string(data));
}

void StringParser<unsigned long>::parse(std::string_view data, unsigned long &value)
{
    value = std::stoul(std::string(data));
}

void StringParser<long long>::parse(std::string_view data, long long &value)
{
    value = std::stoll(std::string(data));
}

void StringParser<unsigned long long>::parse(std::string_view data, unsigned long long &value)
{
    value = std::stoull(std::string(data));
}

void StringParser<float>::parse(std::string_view data, float &value)
{
    value = std::stof(std::string(data));
}

void StringParser<double>::parse(std::string_view data, double &value)
{
    value = std::stod(std::string(data));
}

void StringParser<long double>::parse(std::string_view data, long double &value)
{
    value = std::stold(std::string(data));
}
} // namespace brayns

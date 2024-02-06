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

#include "StringParser.h"

#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace
{
template<typename T>
struct StlParser
{
    static T parse(const std::string &data, size_t &index)
    {
        static_assert(std::is_void_v<T>, "Unsupported STL type");
    }
};

template<>
struct StlParser<int>
{
    static int parse(const std::string &data, size_t &index)
    {
        return std::stoi(data, &index);
    }
};

template<>
struct StlParser<long>
{
    static long parse(const std::string &data, size_t &index)
    {
        return std::stol(data, &index);
    }
};

template<>
struct StlParser<unsigned long>
{
    static unsigned long parse(const std::string &data, size_t &index)
    {
        return std::stoul(data, &index);
    }
};

template<>
struct StlParser<long long>
{
    static long long parse(const std::string &data, size_t &index)
    {
        return std::stoll(data, &index);
    }
};

template<>
struct StlParser<unsigned long long>
{
    static unsigned long long parse(const std::string &data, size_t &index)
    {
        return std::stoull(data, &index);
    }
};

template<>
struct StlParser<float>
{
    static float parse(const std::string &data, size_t &index)
    {
        return std::stof(data, &index);
    }
};

template<>
struct StlParser<double>
{
    static double parse(const std::string &data, size_t &index)
    {
        return std::stod(data, &index);
    }
};

template<>
struct StlParser<long double>
{
    static long double parse(const std::string &data, size_t &index)
    {
        return std::stold(data, &index);
    }
};

class StlParserHelper
{
public:
    template<typename T>
    static T parse(const std::string &data, size_t &index)
    {
        try
        {
            return StlParser<T>::parse(data, index);
        }
        catch (const std::invalid_argument &)
        {
            throw std::invalid_argument("Cannot parse '" + data + "' as a number");
        }
        catch (const std::out_of_range &)
        {
            throw std::out_of_range("Value '" + data + "' out of range");
        }
    }
};

class NumberValidator
{
public:
    static void enforceFullMatch(const std::string &data, size_t index)
    {
        if (index != data.size())
        {
            throw std::invalid_argument("Invalid numeric characters in '" + data + "'");
        }
    }

    template<typename T, typename U>
    static void checkLimits(const std::string &data, U value)
    {
        static_assert(sizeof(U) >= sizeof(T));
        auto min = static_cast<U>(std::numeric_limits<T>::lowest());
        if (value < min)
        {
            throw std::out_of_range("Out of range: " + data + " < " + std::to_string(min));
        }
        auto max = static_cast<U>(std::numeric_limits<T>::max());
        if (value > max)
        {
            throw std::out_of_range("Out of range: " + data + " > " + std::to_string(max));
        }
    }
};

class NumberParser
{
public:
    template<typename T, typename U>
    static void parse(std::string_view data, U &value)
    {
        auto buffer = std::string(data);
        auto number = _parseStl<T>(buffer);
        if constexpr (std::is_same_v<T, U>)
        {
            value = number;
            return;
        }
        NumberValidator::checkLimits<U>(buffer, number);
        value = static_cast<U>(number);
    }

private:
    template<typename T>
    static T _parseStl(const std::string &data)
    {
        size_t index = 0;
        auto number = StlParserHelper::parse<T>(data, index);
        NumberValidator::enforceFullMatch(data, index);
        return number;
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
    NumberParser::parse<int>(data, value);
}

void StringParser<int8_t>::parse(std::string_view data, int8_t &value)
{
    NumberParser::parse<int>(data, value);
}

void StringParser<uint8_t>::parse(std::string_view data, uint8_t &value)
{
    NumberParser::parse<unsigned long>(data, value);
}

void StringParser<int16_t>::parse(std::string_view data, int16_t &value)
{
    NumberParser::parse<int>(data, value);
}

void StringParser<uint16_t>::parse(std::string_view data, uint16_t &value)
{
    NumberParser::parse<unsigned long>(data, value);
}

void StringParser<int32_t>::parse(std::string_view data, int32_t &value)
{
    NumberParser::parse<int32_t>(data, value);
}

void StringParser<uint32_t>::parse(std::string_view data, uint32_t &value)
{
    NumberParser::parse<unsigned long>(data, value);
}

void StringParser<int64_t>::parse(std::string_view data, int64_t &value)
{
    NumberParser::parse<int64_t>(data, value);
}

void StringParser<uint64_t>::parse(std::string_view data, uint64_t &value)
{
    NumberParser::parse<uint64_t>(data, value);
}

void StringParser<float>::parse(std::string_view data, float &value)
{
    NumberParser::parse<float>(data, value);
}

void StringParser<double>::parse(std::string_view data, double &value)
{
    NumberParser::parse<double>(data, value);
}
} // namespace brayns

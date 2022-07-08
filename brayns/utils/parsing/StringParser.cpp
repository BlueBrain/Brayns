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

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace
{
class NumberParser
{
public:
    template<typename T>
    static void parse(std::string_view data, T &value)
    {
        auto buffer = std::string(data);
        size_t index = 0;
        auto number = _parseDouble(buffer, index);
        _enforceFullMatch(buffer, index);
        _checkLimits<T>(buffer, number);
        if (std::is_integral_v<T>)
        {
            _checkIsInteger(buffer, number);
        }
        value = static_cast<T>(number);
    }

private:
    static double _parseDouble(const std::string &data, size_t &index)
    {
        try
        {
            return std::stod(data, &index);
        }
        catch (const std::invalid_argument &)
        {
            throw std::invalid_argument("Cannot parse '" + data + "' as a number");
        }
        catch (const std::out_of_range &)
        {
            throw std::out_of_range("Value '" + data + "' outside 64 bits range");
        }
    }

    static void _enforceFullMatch(const std::string &data, size_t index)
    {
        if (index != data.size())
        {
            throw std::invalid_argument("Invalid numeric characters in '" + data + "'");
        }
    }

    static void _checkIsInteger(const std::string &data, double value)
    {
        if (std::floor(value) != value)
        {
            throw std::invalid_argument("Cannot parse '" + data + "' as an integer");
        }
    }

    template<typename T>
    static void _checkLimits(const std::string &data, double value)
    {
        auto min = static_cast<double>(std::numeric_limits<T>::min());
        if (value < min)
        {
            throw std::out_of_range("Out of range: '" + data + "' < " + std::to_string(min));
        }
        auto max = static_cast<double>(std::numeric_limits<T>::max());
        if (value > max)
        {
            throw std::out_of_range("Out of range: '" + data + "' > " + std::to_string(max));
        }
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

void StringParser<int8_t>::parse(std::string_view data, int8_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<uint8_t>::parse(std::string_view data, uint8_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<int16_t>::parse(std::string_view data, int16_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<uint16_t>::parse(std::string_view data, uint16_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<int32_t>::parse(std::string_view data, int32_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<uint32_t>::parse(std::string_view data, uint32_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<int64_t>::parse(std::string_view data, int64_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<uint64_t>::parse(std::string_view data, uint64_t &value)
{
    NumberParser::parse(data, value);
}

void StringParser<float>::parse(std::string_view data, float &value)
{
    NumberParser::parse(data, value);
}

void StringParser<double>::parse(std::string_view data, double &value)
{
    NumberParser::parse(data, value);
}
} // namespace brayns

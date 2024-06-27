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

#include <fmt/format.h>

namespace
{
template<typename T>
struct NumberParser;

template<>
struct NumberParser<int>
{
    static int parse(const std::string &data, size_t &index)
    {
        return std::stoi(data, &index);
    }
};

template<>
struct NumberParser<long>
{
    static long parse(const std::string &data, size_t &index)
    {
        return std::stol(data, &index);
    }
};

template<>
struct NumberParser<unsigned long>
{
    static unsigned long parse(const std::string &data, size_t &index)
    {
        return std::stoul(data, &index);
    }
};

template<>
struct NumberParser<long long>
{
    static long long parse(const std::string &data, size_t &index)
    {
        return std::stoll(data, &index);
    }
};

template<>
struct NumberParser<unsigned long long>
{
    static unsigned long long parse(const std::string &data, size_t &index)
    {
        return std::stoull(data, &index);
    }
};

template<>
struct NumberParser<float>
{
    static float parse(const std::string &data, size_t &index)
    {
        return std::stof(data, &index);
    }
};

template<>
struct NumberParser<double>
{
    static double parse(const std::string &data, size_t &index)
    {
        return std::stod(data, &index);
    }
};

template<>
struct NumberParser<long double>
{
    static long double parse(const std::string &data, size_t &index)
    {
        return std::stold(data, &index);
    }
};

template<typename T>
T parseNumber(const std::string &data, size_t &index)
{
    try
    {
        return NumberParser<T>::parse(data, index);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument(fmt::format("Cannot parse '{}' as a number", data));
    }
    catch (const std::out_of_range &)
    {
        throw std::out_of_range(fmt::format("Value '{}' out of range", data));
    }
    catch (...)
    {
        throw std::runtime_error("Unknown error while parsing number from string");
    }
}

void enforceFullMatch(const std::string &data, size_t index)
{
    if (index != data.size())
    {
        throw std::invalid_argument(fmt::format("Invalid numeric characters in '{}'", data));
    }
}

template<typename T, typename U>
void checkLimits(U value)
{
    static_assert(sizeof(U) >= sizeof(T));

    auto min = static_cast<U>(std::numeric_limits<T>::lowest());

    if (value < min)
    {
        throw std::out_of_range(fmt::format("Value {} < {}", value, min));
    }

    auto max = static_cast<U>(std::numeric_limits<T>::max());

    if (value > max)
    {
        throw std::out_of_range(fmt::format("Value {} > {}", value, max));
    }
}

template<typename T, typename U>
T parseAs(std::string_view data)
{
    auto buffer = std::string(data);

    auto index = std::size_t(0);
    auto number = parseNumber<U>(buffer, index);

    enforceFullMatch(buffer, index);

    if constexpr (std::is_same_v<T, U>)
    {
        return number;
    }

    checkLimits<T>(number);

    return static_cast<T>(number);
}
}

namespace brayns::experimental
{
bool StringParser<bool>::parse(std::string_view data)
{
    if (data == "1" || data == "true")
    {
        return true;
    }
    if (data == "0" || data == "false")
    {
        return false;
    }
    throw std::invalid_argument(fmt::format("Cannot parse boolean from '{}'", data));
}

std::int8_t StringParser<std::int8_t>::parse(std::string_view data)
{
    return parseAs<std::int8_t, int>(data);
}

std::uint8_t StringParser<std::uint8_t>::parse(std::string_view data)
{
    return parseAs<std::uint8_t, int>(data);
}

std::int16_t StringParser<std::int16_t>::parse(std::string_view data)
{
    return parseAs<std::int16_t, int>(data);
}

std::uint16_t StringParser<std::uint16_t>::parse(std::string_view data)
{
    return parseAs<std::uint16_t, int>(data);
}

std::int32_t StringParser<std::int32_t>::parse(std::string_view data)
{
    return parseAs<std::int32_t, std::int32_t>(data);
}

std::uint32_t StringParser<std::uint32_t>::parse(std::string_view data)
{
    return parseAs<std::uint32_t, unsigned long>(data);
}

std::int64_t StringParser<std::int64_t>::parse(std::string_view data)
{
    return parseAs<std::int64_t, std::int64_t>(data);
}

std::uint64_t StringParser<std::uint64_t>::parse(std::string_view data)
{
    return parseAs<std::uint64_t, std::uint64_t>(data);
}

float StringParser<float>::parse(std::string_view data)
{
    return parseAs<float, float>(data);
}

double StringParser<double>::parse(std::string_view data)
{
    return parseAs<double, double>(data);
}
}

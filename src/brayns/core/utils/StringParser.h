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

#include <concepts>
#include <string_view>

namespace brayns
{
template<typename T>
struct StringParser;

template<typename T>
concept ParsableFromString = std::same_as<T, decltype(StringParser<T>::parse(std::string_view()))>;

template<ParsableFromString T>
T parseStringAs(std::string_view data)
{
    return StringParser<T>::parse(data);
}

template<>
struct StringParser<bool>
{
    static bool parse(std::string_view data);
};

template<>
struct StringParser<std::int8_t>
{
    static std::int8_t parse(std::string_view data);
};

template<>
struct StringParser<std::uint8_t>
{
    static std::uint8_t parse(std::string_view data);
};

template<>
struct StringParser<std::int16_t>
{
    static std::int16_t parse(std::string_view data);
};

template<>
struct StringParser<std::uint16_t>
{
    static std::uint16_t parse(std::string_view data);
};

template<>
struct StringParser<std::int32_t>
{
    static std::int32_t parse(std::string_view data);
};

template<>
struct StringParser<std::uint32_t>
{
    static std::uint32_t parse(std::string_view data);
};

template<>
struct StringParser<std::int64_t>
{
    static std::int64_t parse(std::string_view data);
};

template<>
struct StringParser<std::uint64_t>
{
    static std::uint64_t parse(std::string_view data);
};

template<>
struct StringParser<float>
{
    static float parse(std::string_view data);
};

template<>
struct StringParser<double>
{
    static double parse(std::string_view data);
};
}

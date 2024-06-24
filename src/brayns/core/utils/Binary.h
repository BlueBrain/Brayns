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

#include <bit>
#include <cassert>
#include <cstring>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

namespace brayns::experimental
{
static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

template<typename T>
std::string_view asBytes(const T &value)
{
    auto data = reinterpret_cast<const char *>(&value);
    return {data, data + sizeof(T)};
}

template<typename T>
std::span<char> asBytes(T &value)
{
    auto data = reinterpret_cast<char *>(&value);
    return {data, data + sizeof(T)};
}

template<typename T>
T swapBytes(T value)
{
    auto bytes = asBytes(value);
    std::ranges::reverse(bytes);
    return value;
}

inline std::string_view extractBytes(std::string_view &bytes, std::size_t count)
{
    auto extracted = bytes.substr(0, count);
    bytes.remove_prefix(extracted.size());
    return extracted;
}

template<typename T>
T extractBytesAs(std::string_view &bytes, std::endian bytesEndian)
{
    static constexpr auto size = sizeof(T);

    assert(bytes.size() >= size);

    auto extracted = extractBytes(bytes, size);

    T value;
    std::memcpy(&value, extracted.data(), size);

    if (bytesEndian != std::endian::native)
    {
        return swapBytes(value);
    }

    return value;
}

template<typename T>
T parseBytesAs(std::string_view bytes, std::endian bytesEndian)
{
    return extractBytesAs<T>(bytes, bytesEndian);
}

template<typename T>
void composeBytes(T value, std::endian outputEndian, std::string &output)
{
    auto bytes = asBytes(value);

    if (outputEndian != std::endian::native)
    {
        std::ranges::reverse(bytes);
    }

    output.append(bytes.data(), bytes.size());
}

template<typename T>
std::string composeBytes(T value, std::endian outputEndian)
{
    auto bytes = std::string();
    composeBytes(value, outputEndian, bytes);
    return bytes;
}
}

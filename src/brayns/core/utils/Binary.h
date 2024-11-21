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
#include <vector>

#include "Math.h"

namespace brayns
{
static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

template<typename T>
std::span<const char> asBytes(const T &value)
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
void swapBytes(T &value)
{
    auto bytes = asBytes(value);
    std::ranges::reverse(bytes);
}

inline std::span<const char> extractBytes(std::span<const char> &bytes, std::size_t count)
{
    auto extracted = bytes.subspan(0, count);
    bytes = bytes.subspan(extracted.size());
    return extracted;
}

template<typename T>
void extractBytesAsPrimitive(std::span<const char> &bytes, std::endian endian, T &output)
{
    static constexpr auto size = sizeof(T);

    assert(bytes.size() >= size);

    auto extracted = extractBytes(bytes, size);

    std::memcpy(&output, extracted.data(), size);

    if (endian != std::endian::native)
    {
        swapBytes(output);
    }
}

template<typename T>
void composeBytesAsPrimtive(const T &value, std::endian endian, std::vector<char> &output)
{
    auto bytes = asBytes(value);

    output.insert(output.end(), bytes.begin(), bytes.end());

    if (endian != std::endian::native)
    {
        auto offset = static_cast<std::ptrdiff_t>(output.size() - bytes.size());

        auto first = output.begin() + offset;
        auto last = output.end();

        auto appended = std::span<char>(first, last);
        std::ranges::reverse(appended);
    }
}

template<typename T>
struct BinaryReflector
{
    static std::size_t getSize()
    {
        return sizeof(T);
    }

    static void extract(std::span<const char> &bytes, std::endian endian, T &output)
    {
        extractBytesAsPrimitive<T>(bytes, endian, output);
    }

    static void compose(const T &value, std::endian endian, std::vector<char> &output)
    {
        composeBytesAsPrimtive(value, endian, output);
    }
};

template<typename T>
std::size_t getBinarySize()
{
    return BinaryReflector<std::remove_const_t<T>>::getSize();
}

template<typename T>
void extractBytesTo(std::span<const char> &bytes, std::endian endian, T &output)
{
    BinaryReflector<T>::extract(bytes, endian, output);
}

template<typename T>
T extractBytesAs(std::span<const char> &bytes, std::endian endian)
{
    T value;
    extractBytesTo(bytes, endian, value);
    return value;
}

template<typename T>
void parseBytesTo(std::span<const char> bytes, std::endian endian, T &output)
{
    return extractBytesTo<T>(bytes, endian, output);
}

template<typename T>
T parseBytesAs(std::span<const char> bytes, std::endian endian)
{
    return extractBytesAs<T>(bytes, endian);
}

template<typename T>
void composeBytesTo(const T &value, std::endian endian, std::vector<char> &output)
{
    BinaryReflector<T>::compose(value, endian, output);
}

template<typename T>
std::vector<char> composeBytes(const T &value, std::endian endian)
{
    auto output = std::vector<char>();
    BinaryReflector<T>::compose(value, endian, output);
    return output;
}

template<typename T, int S>
struct BinaryReflector<Vector<T, S>>
{
    static constexpr auto componentCount = static_cast<std::size_t>(S);

    static std::size_t getSize()
    {
        return componentCount * sizeof(T);
    }

    static void extract(std::span<const char> &bytes, std::endian endian, Vector<T, S> &output)
    {
        for (auto i = std::size_t(0); i < componentCount; ++i)
        {
            extractBytesTo(bytes, endian, output[i]);
        }
    }

    static void compose(const Vector<T, S> &value, std::endian endian, std::vector<char> &output)
    {
        for (auto i = std::size_t(0); i < componentCount; ++i)
        {
            composeBytesTo(value[i], endian, output);
        }
    }
};

template<>
struct BinaryReflector<Quaternion>
{
    static std::size_t getSize()
    {
        return 4 * sizeof(float);
    }

    static void extract(std::span<const char> &bytes, std::endian endian, Quaternion &output)
    {
        extractBytesTo(bytes, endian, output.i);
        extractBytesTo(bytes, endian, output.j);
        extractBytesTo(bytes, endian, output.k);
        extractBytesTo(bytes, endian, output.r);
    }

    static void compose(const Quaternion &value, std::endian endian, std::vector<char> &output)
    {
        composeBytesTo(value.i, endian, output);
        composeBytesTo(value.j, endian, output);
        composeBytesTo(value.k, endian, output);
        composeBytesTo(value.r, endian, output);
    }
};
}

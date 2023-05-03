/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <array>
#include <string_view>
#include <vector>

#include <brayns/utils/MathTypes.h>
#include <brayns/utils/binary/ByteParser.h>
#include <brayns/utils/string/StringExtractor.h>

namespace brayns
{
template<typename T>
struct ChunkExtractor
{
    static void extract(std::string_view &data, T &value, std::endian endian)
    {
        auto chunk = StringExtractor::extract(data, sizeof(T));
        ByteParser<T>::parse(chunk, value, endian);
    }
};

template<typename T>
struct ChunkExtractor<std::vector<T>>
{
    static void extract(std::string_view &data, std::vector<T> &values, std::endian endian)
    {
        while (!data.empty())
        {
            auto &value = values.emplace_back();
            ChunkExtractor<T>::extract(data, value, endian);
        }
    }
};

template<typename T, size_t S>
struct ChunkExtractor<std::array<T, S>>
{
    static void extract(std::string_view &data, std::array<T, S> &values, std::endian endian)
    {
        for (auto &value : values)
        {
            ChunkExtractor<T>::extract(data, value, endian);
        }
    }
};

template<glm::length_t S, typename T>
struct ChunkExtractor<glm::vec<S, T>>
{
    static void extract(std::string_view &data, glm::vec<S, T> &value, std::endian endian)
    {
        for (glm::length_t i = 0; i < S; ++i)
        {
            ChunkExtractor<T>::extract(data, value[i], endian);
        }
    }
};

template<typename T>
struct ChunkExtractor<glm::qua<T>>
{
    static void extract(std::string_view &data, glm::qua<T> &value, std::endian endian)
    {
        for (glm::length_t i = 0; i < 4; ++i)
        {
            ChunkExtractor<T>::extract(data, value[i], endian);
        }
    }
};
} // namespace brayns

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
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringInfo.h>
#include <brayns/utils/string/StringParser.h>

namespace brayns
{
template<typename T>
struct TokenExtractor
{
    static void extract(std::string_view &data, T &value)
    {
        auto token = StringExtractor::extractToken(data);
        StringParser<T>::parse(token, value);
    }
};

template<typename T>
struct TokenExtractor<std::vector<T>>
{
    static void extract(std::string_view &data, std::vector<T> &values)
    {
        while (!StringInfo::isSpace(data))
        {
            auto &value = values.emplace_back();
            TokenExtractor<T>::extract(data, value);
        }
    }
};

template<typename T, size_t S>
struct TokenExtractor<std::array<T, S>>
{
    static void extract(std::string_view &data, std::array<T, S> &values)
    {
        for (auto &value : values)
        {
            TokenExtractor<T>::extract(data, value);
        }
    }
};

template<typename T, int S>
struct TokenExtractor<math::vec_t<T, S>>
{
    static void extract(std::string_view &data, math::vec_t<T, S> &value)
    {
        constexpr auto limit = static_cast<std::size_t>(S);
        for (std::size_t i = 0; i < limit; ++i)
        {
            TokenExtractor<T>::extract(data, value[i]);
        }
    }
};

template<typename T>
struct TokenExtractor<math::QuaternionT<T>>
{
    static void extract(std::string_view &data, math::QuaternionT<T> &value)
    {
        auto components = &value.i;
        for (std::size_t i = 0; i < 4; ++i)
        {
            TokenExtractor<T>::extract(data, components[i]);
        }
    }
};
} // namespace brayns

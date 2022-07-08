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

#include <array>
#include <string_view>
#include <vector>

#include "StringParser.h"
#include "StringStream.h"

namespace brayns
{
template<typename T>
struct TokenParser
{
    static void parse(StringStream &stream, T &value)
    {
        auto token = stream.extractToken();
        StringParser<T>::parse(token, value);
    }
};

template<typename T>
struct TokenParser<std::vector<T>>
{
    static void parse(StringStream &stream, std::vector<T> &values)
    {
        while (!stream.isSpace())
        {
            auto &value = values.emplace_back();
            TokenParser<T>::parse(stream, value);
        }
    }
};

template<typename T, size_t S>
struct TokenParser<std::array<T, S>>
{
    static void parse(StringStream &stream, std::array<T, S> &values)
    {
        for (auto &value : values)
        {
            TokenParser<T>::parse(stream, value);
        }
    }
};
} // namespace brayns

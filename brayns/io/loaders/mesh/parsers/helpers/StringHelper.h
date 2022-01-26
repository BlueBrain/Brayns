/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <sstream>
#include <stdexcept>
#include <string_view>

namespace brayns
{
class StringHelper
{
public:
    static bool isSpace(char c);

    static bool isSpace(std::string_view str);

    static bool startsWith(std::string_view str, std::string_view item);

    static std::string_view trimLeft(std::string_view str);

    static size_t count(std::string_view str, std::string_view item);

    static std::string_view extract(std::string_view &str, std::string_view separator);

    static std::string_view extractToken(std::string_view &str);

    static size_t countTokens(std::string_view str);

    template<typename T>
    static T extract(std::string_view &data)
    {
        auto token = extractToken(data);
        auto buffer = std::string(token);
        std::istringstream stream(buffer);
        T value;
        stream >> value;
        if (stream.fail())
        {
            throw std::runtime_error("Failed to parse value '" + buffer + "'");
        }
        return value;
    }
};

template<>
int8_t StringHelper::extract<int8_t>(std::string_view &data);

template<>
uint8_t StringHelper::extract<uint8_t>(std::string_view &data);
} // namespace brayns

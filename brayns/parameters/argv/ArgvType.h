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

#include <type_traits>

#include <brayns/utils/EnumInfo.h>

namespace brayns
{
enum class ArgvType
{
    String,
    Boolean,
    Integer,
    Number
};

template<>
struct EnumReflector<ArgvType>
{
    static EnumMap<ArgvType> reflect()
    {
        return {
            {"string", ArgvType::String},
            {"boolean", ArgvType::Boolean},
            {"integer", ArgvType::Integer},
            {"number", ArgvType::Number}};
    }
};

struct ArgvTypeInfo
{
    template<typename T>
    static constexpr ArgvType getType()
    {
        if constexpr (std::is_enum_v<T>)
        {
            return ArgvType::String;
        }
        if constexpr (std::is_same_v<T, bool>)
        {
            return ArgvType::Boolean;
        }
        if constexpr (std::is_integral_v<T>)
        {
            return ArgvType::Integer;
        }
        if constexpr (std::is_arithmetic_v<T>)
        {
            return ArgvType::Number;
        }
        return ArgvType::String;
    }

    template<typename T>
    static constexpr bool isNumeric()
    {
        if constexpr (std::is_enum_v<T>)
        {
            return false;
        }
        if constexpr (std::is_same_v<T, bool>)
        {
            return false;
        }
        return std::is_arithmetic_v<T>;
    }
};
} // namespace brayns

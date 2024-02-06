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

#include <sstream>

#include <brayns/utils/MathTypes.h>

#include "ArgvProperty.h"

namespace brayns
{
class ArgvExtractor
{
public:
    template<typename T>
    static void extract(const ArgvValue &value, T &result)
    {
        if constexpr (std::is_enum_v<T>)
        {
            result = EnumInfo::getValue<T>(value.toString());
            return;
        }
        if constexpr (std::is_same_v<T, bool>)
        {
            result = value.toBoolean();
            return;
        }
        if constexpr (std::is_integral_v<T>)
        {
            result = static_cast<T>(value.toInteger());
            return;
        }
        if constexpr (std::is_arithmetic_v<T>)
        {
            result = static_cast<T>(value.toNumber());
            return;
        }
        if constexpr (std::is_same_v<T, std::string>)
        {
            result = value.toString();
            return;
        }
        throw std::runtime_error("Invalid type");
    }

    template<typename T>
    static void extractVector(const std::vector<ArgvValue> &values, std::vector<T> &result)
    {
        auto size = values.size();
        result.resize(size);
        for (size_t i = 0; i < size; ++i)
        {
            extract(values[i], result[i]);
        }
    }

    template<typename T, int S>
    static void extractMathVector(const std::vector<ArgvValue> &values, math::vec_t<T, S> &result)
    {
        constexpr auto limit = static_cast<std::size_t>(S);
        for (std::size_t i = 0; i < limit; ++i)
        {
            auto index = static_cast<size_t>(i);
            extract<T>(values[index], result[i]);
        }
    }
};

class ArgvStringifier
{
public:
    template<typename T>
    static std::string stringify(const T &value)
    {
        if constexpr (std::is_enum_v<T>)
        {
            return EnumInfo::getName(value);
        }
        if constexpr (std::is_same_v<T, bool>)
        {
            return value ? "true" : "false";
        }
        if constexpr (std::is_same_v<T, std::string>)
        {
            return value;
        }
        if constexpr (std::is_arithmetic_v<T>)
        {
            return std::to_string(value);
        }
        throw std::runtime_error("Invalid type");
    }

    template<typename T>
    static std::string stringifyVector(const std::vector<T> &values)
    {
        std::ostringstream stream;
        bool first = true;
        for (const auto &value : values)
        {
            if (!first)
            {
                stream << " ";
            }
            first = false;
            stream << stringify(value);
        }
        return stream.str();
    }

    template<typename T, int S>
    static std::string stringifyMathVector(const math::vec_t<T, S> &value)
    {
        std::ostringstream stream;
        bool first = true;
        for (std::size_t i = 0; i < S; ++i)
        {
            if (!first)
            {
                stream << " ";
            }
            first = false;
            stream << value[i];
        }
        return stream.str();
    }
};

template<typename T>
struct ArgvReflector
{
    static ArgvProperty reflect(T &value)
    {
        auto property = GetArgvProperty::of<T>();
        property.load = [&](const auto &values) { ArgvExtractor::extract(values[0], value); };
        property.stringify = [&] { return ArgvStringifier::stringify(value); };
        property.minItems = 1;
        property.maxItems = 1;
        return property;
    }
};

template<typename T>
struct ArgvReflector<std::vector<T>>
{
    static ArgvProperty reflect(std::vector<T> &value)
    {
        auto property = GetArgvProperty::of<T>();
        property.load = [&](const auto &values) { ArgvExtractor::extractVector(values, value); };
        property.stringify = [&] { return ArgvStringifier::stringifyVector(value); };
        property.multitoken = true;
        return property;
    }
};

template<typename T, int S>
struct ArgvReflector<math::vec_t<T, S>>
{
    static ArgvProperty reflect(math::vec_t<T, S> &value)
    {
        auto property = GetArgvProperty::of<T>();
        property.load = [&](const auto &values) { ArgvExtractor::extractMathVector<T, S>(values, value); };
        property.stringify = [&] { return ArgvStringifier::stringifyMathVector(value); };
        property.multitoken = true;
        property.minItems = S;
        property.maxItems = S;
        return property;
    }
};
} // namespace brayns

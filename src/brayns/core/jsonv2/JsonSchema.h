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
#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <brayns/core/utils/EnumReflector.h>

#include "JsonValue.h"

namespace brayns::experimental
{
enum class JsonType
{
    Unknown,
    Undefined,
    Null,
    Boolean,
    Integer,
    Number,
    String,
    Array,
    Object,
};

template<>
struct EnumReflector<JsonType>
{
    static EnumInfo<JsonType> reflect();
};

constexpr bool isNumeric(JsonType type)
{
    return type == JsonType::Integer || type == JsonType::Number;
}

constexpr bool isPrimitive(JsonType type)
{
    return type >= JsonType::Undefined && type <= JsonType::String;
}

template<typename T>
struct JsonTypeReflector
{
    static inline constexpr auto type = JsonType::Unknown;
};

template<>
struct JsonTypeReflector<JsonValue>
{
    static inline constexpr auto type = JsonType::Undefined;
};

template<>
struct JsonTypeReflector<NullJson>
{
    static inline constexpr auto type = JsonType::Null;
};

template<>
struct JsonTypeReflector<bool>
{
    static inline constexpr auto type = JsonType::Boolean;
};

template<std::integral T>
struct JsonTypeReflector<T>
{
    static inline constexpr auto type = JsonType::Integer;
};

template<std::floating_point T>
struct JsonTypeReflector<T>
{
    static inline constexpr auto type = JsonType::Number;
};

template<>
struct JsonTypeReflector<std::string>
{
    static inline constexpr auto type = JsonType::String;
};

template<typename T>
constexpr JsonType jsonTypeOf = JsonTypeReflector<T>::type;

JsonType getJsonType(const JsonValue &json);

struct RequiredJsonType
{
    JsonType value;

    void throwIfNotCompatible(JsonType type);

    constexpr bool isCompatible(JsonType type)
    {
        if (value == JsonType::Unknown || type == JsonType::Unknown)
        {
            return false;
        }
        if (type == value)
        {
            return true;
        }
        if (value == JsonType::Undefined)
        {
            return true;
        }
        if (value == JsonType::Number && type == JsonType::Integer)
        {
            return true;
        }
        return false;
    }
};

template<typename T>
void throwIfNotCompatible(const JsonValue &json)
{
    auto type = getJsonType(json);
    auto required = RequiredJsonType{jsonTypeOf<T>};
    required.throwIfNotCompatible(type);
}

struct JsonSchema
{
    std::string description = {};
    bool required = true;
    JsonValue defaultValue = {};
    std::vector<JsonSchema> oneOf = {};
    JsonType type = JsonType::Undefined;
    std::string constant = {};
    std::optional<double> minimum = {};
    std::optional<double> maximum = {};
    std::vector<JsonSchema> items = {};
    std::optional<std::size_t> minItems = {};
    std::optional<std::size_t> maxItems = {};
    std::map<std::string, JsonSchema> properties = {};

    auto operator<=>(const JsonSchema &) const = default;
};
}

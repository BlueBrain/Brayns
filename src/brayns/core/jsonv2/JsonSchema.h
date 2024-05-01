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

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <brayns/core/utils/EnumReflector.h>

#include "JsonValue.h"

namespace brayns::experimental
{
enum class JsonType
{
    Undefined,
    Null,
    Boolean,
    Integer,
    Number,
    String,
    Array,
    Object,
};

constexpr bool isNumeric(JsonType type)
{
    return type == JsonType::Integer || type == JsonType::Number;
}

template<>
struct EnumReflector<JsonType>
{
    static EnumMap<JsonType> reflect()
    {
        return {
            {"undefined", JsonType::Undefined},
            {"null", JsonType::Null},
            {"boolean", JsonType::Boolean},
            {"integer", JsonType::Integer},
            {"number", JsonType::Number},
            {"string", JsonType::String},
            {"array", JsonType::Array},
            {"object", JsonType::Object},
        };
    }
};

struct JsonSchema
{
    std::string title;
    std::string description;
    bool required = false;
    JsonValue defaultValue;
    JsonType type = JsonType::Undefined;
    std::optional<double> minimum;
    std::optional<double> maximum;
    std::vector<JsonSchema> items;
    std::optional<std::size_t> minItems;
    std::optional<std::size_t> maxItems;
    std::map<std::string, JsonSchema> properties;
    std::vector<std::string> enums;
    std::vector<JsonSchema> oneOf;

    auto operator<=>(const JsonSchema &) const = default;
};
}

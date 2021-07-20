/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "JsonType.h"

namespace brayns
{
/**
 * @brief Representation of a JSON schema.
 *
 */
struct JsonSchema
{
    std::vector<JsonSchema> oneOf;
    std::string title;
    std::string description;
    JsonType type = JsonType::Null;
    bool readOnly = false;
    bool writeOnly = false;
    JsonValue defaultValue;
    boost::optional<double> minimum;
    boost::optional<double> maximum;
    std::vector<JsonValue> enums;
    std::map<std::string, JsonSchema> properties;
    std::vector<std::string> required;
    std::vector<JsonSchema> additionalProperties;
    std::vector<JsonSchema> items;
    boost::optional<size_t> minItems;
    boost::optional<size_t> maxItems;
};

/**
 * @brief Helper class to get some basic info about a JSON schema.
 *
 */
struct JsonSchemaHelper
{
    static bool isEmpty(const JsonSchema& schema)
    {
        return schema.type == JsonType::Null && schema.oneOf.empty();
    }

    static bool isOneOf(const JsonSchema& schema)
    {
        return !schema.oneOf.empty();
    }

    static bool isNumber(const JsonSchema& schema)
    {
        return JsonTypeHelper::isNumber(schema.type);
    }

    static bool isObject(const JsonSchema& schema)
    {
        return schema.type == JsonType::Object;
    }

    static bool isArray(const JsonSchema& schema)
    {
        return schema.type == JsonType::Array;
    }

    static bool hasProperty(const JsonSchema& schema, const std::string& key)
    {
        auto& properties = schema.properties;
        return properties.find(key) != properties.end();
    }

    static bool isRequired(const JsonSchema& schema, const std::string& key)
    {
        auto& required = schema.required;
        auto first = required.begin();
        auto last = required.end();
        return std::find(first, last, key) != last;
    }

    static bool hasType(const JsonSchema& schema, JsonType type)
    {
        return JsonTypeHelper::isSame(schema.type, type);
    }
};
} // namespace brayns
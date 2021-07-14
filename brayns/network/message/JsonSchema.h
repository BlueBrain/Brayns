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
    std::string title;
    std::string description;
    std::string type;
    std::vector<JsonSchema> oneOf;
    boost::optional<double> minimum;
    boost::optional<double> maximum;
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
struct JsonSchemaInfo
{
    static bool isEmpty(const JsonSchema& schema)
    {
        return schema.type.empty() && schema.oneOf.empty();
    }

    static bool isOneOf(const JsonSchema& schema)
    {
        return !schema.oneOf.empty();
    }

    static bool isNumber(const JsonSchema& schema)
    {
        return JsonTypeNameInfo::isNumber(schema.type);
    }

    static bool isObject(const JsonSchema& schema)
    {
        return schema.type == JsonTypeName::ofObject();
    }

    static bool isArray(const JsonSchema& schema)
    {
        return schema.type == JsonTypeName::ofArray();
    }

    static bool hasProperty(const JsonSchema& schema, const std::string& key)
    {
        return schema.properties.find(key) != schema.properties.end();
    }

    static bool isRequired(const JsonSchema& schema, const std::string& key)
    {
        auto first = schema.required.begin();
        auto last = schema.required.end();
        return std::find(first, last, key) != last;
    }

    static bool hasType(const JsonSchema& schema, const std::string& type)
    {
        return schema.type == type ||
               isNumber(schema) && JsonTypeNameInfo::isNumber(type);
    }
};
} // namespace brayns
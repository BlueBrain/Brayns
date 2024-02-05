/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <algorithm>
#include <map>
#include <memory>
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
    /**
     * @brief Union description, empty if not an union.
     *
     */
    std::vector<JsonSchema> oneOf;

    /**
     * @brief Title of the schema (type name usually).
     *
     */
    std::string title;

    /**
     * @brief Schema description.
     *
     */
    std::string description;

    /**
     * @brief JSON type.
     *
     */
    JsonType type = JsonType::Unknown;

    /**
     * @brief Check if read only.
     *
     */
    bool readOnly = false;

    /**
     * @brief Check if write only.
     *
     */
    bool writeOnly = false;

    /**
     * @brief Default value (null if not set).
     *
     */
    JsonValue defaultValue;

    /**
     * @brief Optional min value if number.
     *
     */
    boost::optional<double> minimum;

    /**
     * @brief Optional max value if number.
     *
     */
    boost::optional<double> maximum;

    /**
     * @brief Enum description, empty if not an enum.
     *
     */
    std::vector<JsonValue> enums;

    /**
     * @brief List of object properties if object, else empty.
     *
     */
    std::map<std::string, JsonSchema> properties;

    /**
     * @brief List of required properties if object, else empty.
     *
     */
    std::vector<std::string> required;

    /**
     * @brief Description of additional properties, can be an empty schema if
     * any property is authorized, empty if not authorized.
     *
     */
    std::vector<JsonSchema> additionalProperties;

    /**
     * @brief Item description if array, else empty.
     *
     */
    std::vector<JsonSchema> items;

    /**
     * @brief Optional min item count if array.
     *
     */
    boost::optional<size_t> minItems;

    /**
     * @brief Optional max item count if array.
     *
     */
    boost::optional<size_t> maxItems;
};

/**
 * @brief Helper class to get some basic info about a JSON schema.
 *
 */
struct JsonSchemaHelper
{
    /**
     * @brief Check if the schema is empty (wildcard).
     *
     * @param schema Schema to check.
     * @return true Empty schema.
     * @return false Non-empty schema.
     */
    static bool isEmpty(const JsonSchema& schema)
    {
        return schema.type == JsonType::Unknown && schema.oneOf.empty();
    }

    /**
     * @brief Check wether the schema is an union.
     *
     * @param schema Schema to check.
     * @return true Union.
     * @return false Not an union.
     */
    static bool isOneOf(const JsonSchema& schema)
    {
        return !schema.oneOf.empty();
    }

    /**
     * @brief Check if the schema describes a number or an integer.
     *
     * @param schema Schema to check.
     * @return true Number or integer.
     * @return false Not numeric.
     */
    static bool isNumeric(const JsonSchema& schema)
    {
        return JsonTypeHelper::isNumeric(schema.type);
    }

    /**
     * @brief Check wether the schema is an enum.
     *
     * @param schema Schema to check.
     * @return true Enum.
     * @return false Not an enum.
     */
    static bool isEnum(const JsonSchema& schema)
    {
        return !schema.enums.empty();
    }

    /**
     * @brief Check if the schema is an object.
     *
     * @param schema Schema to check.
     * @return true Object.
     * @return false Not an object.
     */
    static bool isObject(const JsonSchema& schema)
    {
        return schema.type == JsonType::Object;
    }

    /**
     * @brief Check if the schema is an array.
     *
     * @param schema Schema to check.
     * @return true Array.
     * @return false Not an array.
     */
    static bool isArray(const JsonSchema& schema)
    {
        return schema.type == JsonType::Array;
    }

    /**
     * @brief Check if the schema has the given property.
     *
     * @param schema Schema to check.
     * @param key Key to find in schema properties.
     * @return true Has property.
     * @return false Don't have the property.
     */
    static bool hasProperty(const JsonSchema& schema, const std::string& key)
    {
        auto& properties = schema.properties;
        return properties.find(key) != properties.end();
    }

    /**
     * @brief Check wether the property is required in the given schema.
     *
     * @param schema Schema to check.
     * @param key Property key to test.
     * @return true Property is required.
     * @return false Property is not required.
     */
    static bool isRequired(const JsonSchema& schema, const std::string& key)
    {
        auto& required = schema.required;
        auto first = required.begin();
        auto last = required.end();
        return std::find(first, last, key) != last;
    }

    /**
     * @brief Check if the given schema validate the given type.
     *
     * @param schema Schema to check.
     * @param type Type to check.
     * @return true The type is allowed by schema.
     * @return false The type is not allowed by schema.
     */
    static bool checkType(const JsonSchema& schema, JsonType type)
    {
        return JsonTypeHelper::check(schema.type, type);
    }

    /**
     * @brief Set a wildcard for authorized additional properties.
     *
     * @param schema Schema to update.
     */
    static void allowAnyAdditionalProperty(JsonSchema& schema)
    {
        schema.additionalProperties = {{}};
    }

    /**
     * @brief Add all schema properties to required.
     *
     * @param schema Schema to update.
     */
    static void requireAll(JsonSchema& schema)
    {
        auto& properties = schema.properties;
        auto& required = schema.required;
        required.reserve(properties.size());
        for (const auto& pair : properties)
        {
            auto& name = pair.first;
            required.push_back(name);
        }
    }

    /**
     * @brief Get the schema of null object (not a wildcard).
     *
     * @return JsonSchema Null schema.
     */
    static JsonSchema getNullSchema()
    {
        JsonSchema schema;
        schema.type = JsonType::Null;
        return schema;
    }
};
} // namespace brayns
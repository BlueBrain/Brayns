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

#include "JsonErrors.h"
#include "JsonSchema.h"
#include "JsonTypes.h"

namespace brayns
{
/**
 * @brief Validate a JSON value using a given schema.
 *
 */
class JsonValidator
{
public:
    /**
     * @brief Validate json using schema and store result in errors.
     *
     * @param json JSON to validate.
     * @param schema JSON schema.
     * @param errors Store JSON errors.
     */
    static void validate(const JsonValue &json, const JsonSchema &schema, JsonErrors &errors);
};

/**
 * @brief Helper class to check if JSON types are compatible.
 *
 */
class JsonTypeChecker
{
public:
    static bool check(JsonType type, JsonType required);
    static bool check(JsonType type, JsonType required, JsonErrors &errors);
    static bool check(const JsonValue &json, JsonType required, JsonErrors &errors);
};

/**
 * @brief Helper class to check value range and item count.
 *
 */
class JsonRangeChecker
{
public:
    static bool checkItemCount(size_t size, const JsonRange<size_t> &range, JsonErrors &errors);

    template<typename T>
    static bool checkRange(T value, const JsonRange<T> &range, JsonErrors &errors)
    {
        if (value < range.min)
        {
            errors.add("value below minimum: {} < {}", value, range.min);
            return false;
        }
        if (value > range.max)
        {
            errors.add("value above maximum: {} > {}", value, range.max);
            return false;
        }
        return true;
    }
};

/**
 * @brief Helper class to check object properties.
 *
 */
class JsonPropertyChecker
{
public:
    static bool checkRequired(const JsonObject &json, const ObjectSchema &schema, JsonErrors &errors);
    static bool checkUnknown(const std::string &key, const ObjectSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for primitives.
 *
 * @tparam T Primitive type.
 */
template<typename T>
struct JsonSchemaValidator<PrimitiveSchema<T>>
{
    static void validate(const JsonValue &json, const PrimitiveSchema<T> &schema, JsonErrors &errors)
    {
        JsonTypeChecker::check(json, schema.type, errors);
    }
};

/**
 * @brief JSON validation for numeric types.
 *
 * @tparam T Numeric type.
 */
template<typename T>
struct JsonSchemaValidator<NumericSchema<T>>
{
    static void validate(const JsonValue &json, const NumericSchema<T> &schema, JsonErrors &errors)
    {
        if (!JsonTypeChecker::check(json, schema.type, errors))
        {
            return;
        }
        auto value = json.extract<T>();
        JsonRangeChecker::checkRange(value, schema.range, errors);
    }
};

/**
 * @brief JSON validation for arrays.
 *
 */
template<>
struct JsonSchemaValidator<ArraySchema>
{
    static void validate(const JsonValue &json, const ArraySchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for generic maps.
 *
 */
template<>
struct JsonSchemaValidator<MapSchema>
{
    static void validate(const JsonValue &json, const MapSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for structured objects.
 *
 */
template<>
struct JsonSchemaValidator<ObjectSchema>
{
    static void validate(const JsonValue &json, const ObjectSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for enumerations.
 *
 */
template<>
struct JsonSchemaValidator<EnumSchema>
{
    static void validate(const JsonValue &json, const EnumSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for variants.
 *
 */
template<>
struct JsonSchemaValidator<OneOfSchema>
{
    static void validate(const JsonValue &json, const OneOfSchema &schema, JsonErrors &errors);
};
} // namespace brayns

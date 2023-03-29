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

#include "JsonSchema.h"

namespace brayns
{
class JsonValidator
{
public:
    static void validate(const JsonValue &json, const JsonSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for wildcards.
 *
 */
template<>
struct JsonSchemaValidator<WildcardSchema>
{
    static void validate(const JsonValue &json, const WildcardSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for empty objects.
 *
 */
template<>
struct JsonSchemaValidator<NullSchema>
{
    static void validate(const JsonValue &json, const NullSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for booleans.
 *
 */
template<>
struct JsonSchemaValidator<BooleanSchema>
{
    static void validate(const JsonValue &json, const BooleanSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for integers.
 *
 */
template<>
struct JsonSchemaValidator<IntegerSchema>
{
    static void validate(const JsonValue &json, const IntegerSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for numbers.
 *
 */
template<>
struct JsonSchemaValidator<NumberSchema>
{
    static void validate(const JsonValue &json, const NumberSchema &schema, JsonErrors &errors);
};

/**
 * @brief JSON validation for strings.
 *
 */
template<>
struct JsonSchemaValidator<StringSchema>
{
    static void validate(const JsonValue &json, const StringSchema &schema, JsonErrors &errors);
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

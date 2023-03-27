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

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "JsonErrors.h"
#include "JsonTypes.h"

namespace brayns
{
/**
 * @brief Schema serialization trait.
 *
 * @tparam T Schema type (without options).
 */
template<typename T>
struct JsonSchemaSerializer
{
    static void serialize(T &schema, JsonObject &json)
    {
        schema.serialize(json);
    }
};

/**
 * @brief Schema validation trait.
 *
 * @tparam T Schema type (without options).
 */
template<typename T>
struct JsonSchemaValidator
{
    static void validate(const JsonValue &json, const T &schema, JsonErrors &errors)
    {
        schema.validate(json, errors);
    }
};

/**
 * @brief Interface to access different schema types at runtime.
 *
 */
class IJsonSchema
{
public:
    virtual ~IJsonSchema() = default;

    virtual std::unique_ptr<IJsonSchema> clone() const = 0;
    virtual void serialize(JsonObject &json) const = 0;
    virtual void validate(const JsonValue &json, JsonErrors &errors) const = 0;
};

/**
 * @brief Schema interface implementation for specific schema type.
 *
 * @tparam T Schema type (without options).
 */
template<typename T>
class JsonSchemaInterface : public IJsonSchema
{
public:
    template<typename... Args>
    explicit JsonSchemaInterface(Args &&...args)
        : _schema(std::forward<Args>(args)...)
    {
    }

    virtual std::unique_ptr<IJsonSchema> clone() const override
    {
        return std::make_unique<JsonSchemaInterface<T>>(_schema);
    }

    virtual void serialize(JsonObject &json) const override
    {
        return JsonSchemaSerializer<T>::serialize(_schema, json);
    }

    virtual void validate(const JsonValue &json, JsonErrors &errors) const override
    {
        return JsonSchemaValidator<T>::validate(json, _schema, errors);
    }

private:
    T _schema;
};

/**
 * @brief Wraps a schema interface to have a copyable type.
 *
 */
class JsonSchemaHolder
{
public:
    explicit JsonSchemaHolder(std::unique_ptr<IJsonSchema> schema)
        : _schema(std::move(schema))
    {
    }

    JsonSchemaHolder(const JsonSchemaHolder &other)
        : _schema(other._schema->clone())
    {
    }

    JsonSchemaHolder(JsonSchemaHolder &&) = default;

    JsonSchemaHolder &operator=(const JsonSchemaHolder &other)
    {
        _schema = other._schema->clone();
        return *this;
    }

    JsonSchemaHolder &operator=(JsonSchemaHolder &&) = default;

    void serialize(JsonObject &json) const
    {
        _schema->serialize(json);
    }

    void validate(const JsonValue &json, JsonErrors &errors) const
    {
        _schema->validate(json, errors);
    }

private:
    std::unique_ptr<IJsonSchema> _schema;
};

/**
 * @brief Options common to all JSON schemas.
 *
 */
struct JsonOptions
{
    std::string title;
    std::string description;
    JsonValue defaultValue;
};

/**
 * @brief Full JSON schema with options.
 *
 */
struct JsonSchema
{
    JsonSchemaHolder holder;
    JsonOptions options;
};

/**
 * @brief Helper class to create a full schema from a specific schema type.
 *
 */
class JsonSchemaFactory
{
public:
    template<typename T>
    static JsonSchema create(T schema, JsonOptions options = {})
    {
        auto interface = std::make_unique<JsonSchemaInterface<T>>(std::move(schema));
        auto holder = JsonSchemaHolder(std::move(interface));
        return {std::move(holder), std::move(options)};
    }
};

/**
 * @brief Schema of a primitive type (undefined, null, boolean, string).
 *
 * @tparam T Primitive type.
 */
template<typename T>
struct PrimitiveSchema
{
    JsonType type = JsonTypeInfo::getType<T>();
};

/**
 * @brief Defines a range of allowed values for numeric types.
 *
 * @tparam T Numeric type.
 */
template<typename T>
struct JsonRange
{
    T min = std::numeric_limits<T>::lowest();
    T max = std::numeric_limits<T>::max();
};

/**
 * @brief Schema of numeric values (integer and number).
 *
 * @tparam T Numeric type.
 */
template<typename T>
struct NumericSchema : PrimitiveSchema<T>
{
    JsonRange<T> range;
};

/**
 * @brief Schema of arrays.
 *
 */
struct ArraySchema
{
    JsonSchema items;
    JsonRange<size_t> itemCount;
};

/**
 * @brief Schemas of generic maps.
 *
 */
struct MapSchema
{
    JsonSchema items;
};

/**
 * @brief Schema of structured objects.
 *
 */
struct ObjectSchema
{
    std::map<std::string, JsonSchema> properties;
    std::vector<std::string> required;
};

/**
 * @brief Schema of enumerations.
 *
 */
struct EnumSchema
{
    std::vector<std::string> enums;
};

/**
 * @brief Schema of variants.
 *
 */
struct OneOfSchema
{
    std::vector<JsonSchema> oneOf;
};
} // namespace brayns

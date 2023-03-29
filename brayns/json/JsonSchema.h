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
    static void serialize(const T &schema, JsonObject &json)
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

    const T &get() const
    {
        return _schema;
    }

    T &get()
    {
        return _schema;
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

    template<typename T>
    T &as() const
    {
        auto &schema = dynamic_cast<JsonSchemaInterface<T> &>(*_schema);
        return schema.get();
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
    bool readOnly = false;
    bool writeOnly = false;
};

/**
 * @brief Full JSON schema with options.
 *
 */
struct JsonSchema
{
    JsonSchemaHolder holder;
    JsonOptions options;

    template<typename T>
    static JsonSchema from(T schema, JsonOptions options = {})
    {
        auto interface = std::make_unique<JsonSchemaInterface<T>>(std::move(schema));
        auto holder = JsonSchemaHolder(std::move(interface));
        return JsonSchema(std::move(holder), std::move(options));
    }
};

/**
 * @brief Schema of undefined types.
 *
 */
struct WildcardSchema
{
};

/**
 * @brief Schema of null.
 *
 */
struct NullSchema
{
};

/**
 * @brief Schema of booleans
 *
 */
struct BooleanSchema
{
};

/**
 * @brief Schema of integers.
 *
 */
struct IntegerSchema
{
    double min = 0.0;
    double max = 0.0;
};

/**
 * @brief Schema of numbers.
 *
 */
struct NumberSchema : IntegerSchema
{
};

/**
 * @brief Schema of numbers.
 *
 */
struct StringSchema
{
};

/**
 * @brief Schema of arrays.
 *
 */
struct ArraySchema
{
    JsonSchema items;
    size_t minItems = 0;
    size_t maxItems = 0;
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
    std::vector<std::string> values;
};

/**
 * @brief Schema of variants.
 *
 */
struct OneOfSchema
{
    std::vector<JsonSchema> schemas;
};
} // namespace brayns

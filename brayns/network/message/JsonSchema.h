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
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <brayns/common/mathTypes.h>

#include "Json.h"

namespace brayns
{
/**
 * @brief Helper class to store JSON type names.
 *
 */
class JsonTypeName
{
public:
    static const std::string& ofNull()
    {
        static const std::string name = "null";
        return name;
    }

    static const std::string& ofBoolean()
    {
        static const std::string name = "boolean";
        return name;
    }

    static const std::string& ofInteger()
    {
        static const std::string name = "integer";
        return name;
    }

    static const std::string& ofNumber()
    {
        static const std::string name = "number";
        return name;
    }

    static const std::string& ofString()
    {
        static const std::string name = "string";
        return name;
    }

    static const std::string& ofArray()
    {
        static const std::string name = "array";
        return name;
    }

    static const std::string& ofObject()
    {
        static const std::string name = "object";
        return name;
    }

    static bool isNumber(const std::string& name)
    {
        return name == ofInteger() || name == ofNumber();
    }

    template <typename T>
    static const std::string& ofPrimitive()
    {
        if (std::is_same<T, bool>())
        {
            return ofBoolean();
        }
        if (std::is_integral<T>())
        {
            return ofInteger();
        }
        if (std::is_arithmetic<T>())
        {
            return ofNumber();
        }
        if (std::is_same<T, std::string>())
        {
            return ofString();
        }
        return ofNull();
    }
};

/**
 * @brief Helper class to get JSON type name from a JSON value.
 *
 */
class JsonValueType
{
public:
    static const std::string& of(const JsonValue& json)
    {
        if (json.isBoolean())
        {
            return JsonTypeName::ofBoolean();
        }
        if (json.isInteger())
        {
            return JsonTypeName::ofInteger();
        }
        if (json.isNumeric())
        {
            return JsonTypeName::ofNumber();
        }
        if (json.isString())
        {
            return JsonTypeName::ofString();
        }
        if (json.type() == typeid(JsonArray::Ptr))
        {
            return JsonTypeName::ofArray();
        }
        if (json.type() == typeid(JsonObject::Ptr))
        {
            return JsonTypeName::ofObject();
        }
        return JsonTypeName::ofNull();
    }
};

/**
 * @brief Representation of a JSON schema.
 *
 */
struct JsonSchema
{
    std::string name;
    std::string title;
    std::string description;
    std::string type;
    std::vector<JsonSchema> oneOf;
    boost::optional<double> minimum;
    boost::optional<double> maximum;
    std::vector<JsonSchema> properties;
    bool additionalProperties = false;
    std::vector<std::string> required;
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
        return schema.type == JsonTypeName::ofInteger() ||
               schema.type == JsonTypeName::ofNumber();
    }

    static bool isObject(const JsonSchema& schema)
    {
        return schema.type == JsonTypeName::ofObject();
    }

    static bool isArray(const JsonSchema& schema)
    {
        return schema.type == JsonTypeName::ofArray();
    }

    static bool isRequired(const JsonSchema& schema, const std::string& key)
    {
        auto first = schema.properties.begin();
        auto last = schema.properties.end();
        return std::find_if(first, last,
                            [&](const auto& property)
                            { return property.name == key; }) != last;
    }

    static bool hasType(const JsonSchema& schema, const std::string& type)
    {
        if (schema.type == JsonTypeName::ofNumber())
        {
            return JsonTypeName::isNumber(type);
        }
        return type == schema.type;
    }
};

/**
 * @brief Create JSON schema for a given type (can be specialized).
 *
 * The default implementation returns an empty schema. For basic types (bool,
 * integer, number, string), the type is filled and for unsigned types, a
 * minimum of 0 is set.
 *
 * @tparam T Type to get JSON schema from.
 */
template <typename T>
struct JsonSchemaFactory
{
    static JsonSchema createSchema()
    {
        JsonSchema schema;
        if (JsonType<T>::isPrimitive())
        {
            schema.type = JsonTypeName::ofPrimitive<T>();
        }
        if (!std::is_same<T, bool>() && std::is_unsigned<T>())
        {
            schema.minimum = 0.0;
        }
        return schema;
    }
};

struct JsonProperty
{
    static void remove(JsonSchema& schema, const std::string& name)
    {
        auto& properties = schema.properties;
        properties.erase(std::remove_if(properties.begin(), properties.end(),
                                        [&](const auto& item)
                                        { return item.name == name; }),
                         properties.end());
        auto& required = schema.required;
        required.erase(std::remove(required.begin(), required.end(), name),
                       required.end());
    }

    template <typename T>
    static void add(JsonSchema& schema, const std::string& name)
    {
        auto property = JsonSchemaFactory<T>::createSchema();
        property.name = name;
        schema.properties.push_back(std::move(property));
        schema.required.push_back(name);
    }
};

/**
 * @brief Helper class to create a schema for an array of ItemType.
 *
 * @tparam ItemType Array item type.
 */
template <typename ItemType>
struct JsonArraySchemaFactory
{
    static JsonSchema createSchema()
    {
        JsonSchema schema;
        schema.type = JsonTypeName::ofArray();
        auto itemSchema = JsonSchemaFactory<ItemType>::createSchema();
        schema.items.push_back(std::move(itemSchema));
        return schema;
    }
};

/**
 * @brief Specialization to create JSON schema for vectors.
 *
 * @tparam T Vector item type.
 */
template <typename T>
struct JsonSchemaFactory<std::vector<T>> : public JsonArraySchemaFactory<T>
{
};

/**
 * @brief Specialization to create JSON schema for deque.
 *
 * @tparam T Deque item type.
 */
template <typename T>
struct JsonSchemaFactory<std::deque<T>> : public JsonArraySchemaFactory<T>
{
};

/**
 * @brief Specialization to create JSON schema for GLM vectors.
 *
 * @tparam S Vector size.
 * @tparam T Vector item type.
 */
template <glm::length_t S, typename T>
struct JsonSchemaFactory<glm::vec<S, T>>
{
    static JsonSchema createSchema()
    {
        auto schema = JsonArraySchemaFactory<T>::createSchema();
        schema.minItems = S;
        schema.maxItems = S;
        return schema;
    }
};

/**
 * @brief Specialization to create JSON schema for GLM quaternion.
 *
 * @tparam T Quaternion item type.
 */
template <typename T>
struct JsonSchemaFactory<glm::qua<T>>
{
    static JsonSchema createSchema()
    {
        auto schema = JsonArraySchemaFactory<T>::createSchema();
        schema.minItems = 4;
        schema.maxItems = 4;
        return schema;
    }
};

template<typename T>
struct JsonSchemaFactory<Box<T>>
{
    static JsonSchema createSchema()
    {
        JsonSchema schema;
        schema.title = "Box";
        schema.type = JsonTypeName::ofObject();
        JsonProperty::add<typename Box<T>::vec>(schema, "min");
        JsonProperty::add<typename Box<T>::vec>(schema, "max");
        return schema;
    }
};

template <typename... Types>
using OneOf = boost::variant<Types...>;

template <typename... Types>
struct JsonSchemaFactory<OneOf<Types...>>
{
    static JsonSchema createSchema()
    {
        JsonSchema schema;
        addOneOf<Types...>(schema);
        return schema;
    }

    template <typename T, typename... Others>
    static void addOneOf(JsonSchema& schema)
    {
        addOneOf<T>(schema);
        addOneOf<Others...>(schema);
    }

    template <typename T>
    static void addOneOf(JsonSchema& schema)
    {
        auto oneOf = JsonSchemaFactory<T>::createSchema();
        schema.oneOf.push_back(std::move(oneOf));
    }
};

/**
 * @brief Custom serialization for JSON schemas.
 *
 */
template <>
struct JsonSerializer<JsonSchema>
{
    static bool serialize(const JsonSchema& value, JsonValue& json);
    static bool deserialize(const JsonValue& json, JsonSchema& value);
};
} // namespace brayns
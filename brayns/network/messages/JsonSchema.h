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

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <brayns/common/mathTypes.h>

#include "Json.h"

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
    std::vector<std::pair<std::string, JsonSchema>> properties;
    std::vector<std::string> required;
    std::vector<JsonSchema> items;

    bool isEmpty() const { return type.empty(); }

    bool requires(const std::string& key) const
    {
        for (const auto& value : required)
        {
            if (value == key)
            {
                return true;
            }
        }
        return false;
    }
};

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

    template <typename T>
    static const std::string& ofPrimitive()
    {
        static_assert(JsonType<T>::isPrimitive(), "Not a JSON primitive");
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
class JsonTypeHelper
{
public:
    static const std::string& getJsonTypeName(const JsonValue& json)
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
 * @brief Create JSON schema for a given type (can be specialized).
 *
 * The default implementation is for basic types and hence only have a type.
 *
 * @tparam T Type to get JSON schema from.
 */
template <typename T>
struct JsonSchemaFactory
{
    static_assert(JsonType<T>::isPrimitive(),
                  "JSON schema creation is not supported for this type, please "
                  "provide a specialization of JsonSchemaFactory<T>");

    static JsonSchema createJsonSchema()
    {
        JsonSchema schema;
        schema.type = JsonTypeName::ofPrimitive<T>();
        return schema;
    }
};

/**
 * @brief Empty schema for JSON value (treated as any).
 *
 */
template <>
struct JsonSchemaFactory<JsonValue>
{
    static JsonSchema createJsonSchema() { return {}; }
};

/**
 * @brief Helper class to create a schema for an array of ItemType.
 *
 * @tparam ItemType Array item type.
 */
template <typename ItemType>
struct JsonArraySchema
{
    static JsonSchema create()
    {
        JsonSchema schema;
        schema.type = JsonTypeName::ofArray();
        auto items = JsonSchemaFactory<ItemType>::createJsonSchema();
        schema.items.push_back(std::move(items));
        return schema;
    }
};

/**
 * @brief Specialization to create JSON schema for vectors.
 *
 * @tparam T Vector item type.
 */
template <typename T>
struct JsonSchemaFactory<std::vector<T>>
{
    static JsonSchema createJsonSchema()
    {
        return JsonArraySchema<T>::create();
    }
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
    static JsonSchema createJsonSchema()
    {
        return JsonArraySchema<T>::create();
    }
};

/**
 * @brief Helper to serialize JSON schemas.
 *
 */
class JsonSchemaSerializer
{
public:
    static JsonObject::Ptr serialize(const JsonSchema& schema)
    {
        auto object = Poco::makeShared<JsonObject>();
        _addInfo(schema, *object);
        _addProperties(schema, *object);
        _addItems(schema, *object);
        return object;
    }

private:
    static void _addInfo(const JsonSchema& schema, JsonObject& object)
    {
        if (!schema.title.empty())
        {
            object.set("title", schema.title);
        }
        if (!schema.description.empty())
        {
            object.set("description", schema.description);
        }
        if (!schema.type.empty())
        {
            object.set("type", schema.type);
        }
    }

    static void _addProperties(const JsonSchema& schema, JsonObject& object)
    {
        if (schema.properties.empty())
        {
            return;
        }
        auto properties = Poco::makeShared<JsonObject>();
        for (const auto& pair : schema.properties)
        {
            properties->set(pair.first, Json::serialize(pair.second));
        }
        object.set("properties", properties);
    }

    static void _addItems(const JsonSchema& schema, JsonObject& object)
    {
        if (schema.items.empty())
        {
            return;
        }
        object.set("items", Json::serialize(schema.items[0]));
    }
};

/**
 * @brief Helper to deserialize JSON schemas.
 *
 */
class JsonSchemaDeserializer
{
public:
    static void deserialize(const JsonObject& object, JsonSchema& schema)
    {
        _addInfo(object, schema);
        _addProperties(object, schema);
        _addItems(object, schema);
    }

private:
    static void _addInfo(const JsonObject& object, JsonSchema& schema)
    {
        Json::deserialize(object.get("title"), schema.title);
        Json::deserialize(object.get("description"), schema.description);
        Json::deserialize(object.get("type"), schema.type);
    }

    static void _addProperties(const JsonObject& object, JsonSchema& schema)
    {
        auto properties = object.getObject("properties");
        if (!properties)
        {
            return;
        }
        schema.properties.clear();
        for (const auto& pair : *properties)
        {
            schema.properties.emplace_back();
            auto& property = schema.properties.back();
            property.first = pair.first;
            Json::deserialize(pair.second, property.second);
        }
    }

    static void _addItems(const JsonObject& object, JsonSchema& schema)
    {
        auto items = object.get("items");
        if (items.isEmpty())
        {
            return;
        }
        schema.items.clear();
        schema.items.emplace_back();
        Json::deserialize(items, schema.items.back());
    }
};

/**
 * @brief Custom serialization for JSON schemas.
 *
 */
template <>
struct JsonSerializer<JsonSchema>
{
    static void serialize(const JsonSchema& value, JsonValue& json)
    {
        json = JsonSchemaSerializer::serialize(value);
    }

    static void deserialize(const JsonValue& json, JsonSchema& value)
    {
        if (json.type() != typeid(JsonObject::Ptr))
        {
            return;
        }
        auto& object = *json.extract<JsonObject::Ptr>();
        JsonSchemaDeserializer::deserialize(object, value);
    }
};
} // namespace brayns
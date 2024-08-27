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

#include <concepts>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include <brayns/core/json/JsonReflector.h>

namespace brayns
{
template<typename ParentType>
struct JsonField
{
    std::string name;
    JsonSchema schema;
    std::function<JsonValue(const ParentType &)> serialize;
    std::function<void(const JsonValue &, ParentType &)> deserialize;
};

template<typename T>
class JsonObjectInfo
{
public:
    explicit JsonObjectInfo(std::vector<JsonField<T>> fields, std::string description):
        _fields(std::move(fields)),
        _description(std::move(description))
    {
    }

    JsonSchema getSchema() const
    {
        auto schema = JsonSchema{.description = _description, .type = JsonType::Object};

        for (const auto &field : _fields)
        {
            schema.properties[field.name] = field.schema;
        }

        return schema;
    }

    JsonValue serialize(const T &value) const
    {
        auto object = createJsonObject();

        for (const auto &field : _fields)
        {
            auto jsonItem = field.serialize(value);

            if (jsonItem.isEmpty() && !field.schema.required)
            {
                continue;
            }

            object->set(field.name, jsonItem);
        }

        return object;
    }

    T deserialize(const JsonValue &json) const
    {
        auto value = T{};

        const auto &object = getObject(json);

        for (const auto &field : _fields)
        {
            if (object.has(field.name))
            {
                auto jsonItem = object.get(field.name);
                field.deserialize(jsonItem, value);
                continue;
            }

            if (field.schema.required)
            {
                throw JsonException("Missing required field");
            }

            field.deserialize(field.schema.defaultValue, value);
        }

        return value;
    }

private:
    std::vector<JsonField<T>> _fields;
    std::string _description;
};

template<typename T>
struct JsonObjectReflector;

template<typename T>
concept ReflectedJsonObject = std::same_as<JsonObjectInfo<T>, decltype(JsonObjectReflector<T>::reflect())>;

template<ReflectedJsonObject T>
const JsonObjectInfo<T> &reflectJsonObject()
{
    static const auto info = JsonObjectReflector<T>::reflect();
    return info;
}

template<ReflectedJsonObject T>
struct JsonReflector<T>
{
    static JsonSchema getSchema()
    {
        const auto &info = reflectJsonObject<T>();
        return info.getSchema();
    }

    static JsonValue serialize(const T &value)
    {
        const auto &info = reflectJsonObject<T>();
        return info.serialize(value);
    }

    static T deserialize(const JsonValue &json)
    {
        const auto &info = reflectJsonObject<T>();
        return info.deserialize(json);
    }
};

class JsonFieldBuilder
{
public:
    explicit JsonFieldBuilder(JsonSchema &schema):
        _schema(&schema)
    {
    }

    JsonFieldBuilder description(std::string value)
    {
        _schema->description = std::move(value);
        return *this;
    }

    JsonFieldBuilder required(bool value)
    {
        _schema->required = value;
        return *this;
    }

    JsonFieldBuilder minimum(std::optional<double> value)
    {
        _schema->minimum = value;
        return *this;
    }

    JsonFieldBuilder maximum(std::optional<double> value)
    {
        _schema->maximum = value;
        return *this;
    }

    JsonFieldBuilder uniqueItems(bool value)
    {
        _schema->uniqueItems = value;
        return *this;
    }

    JsonFieldBuilder minItems(std::optional<std::size_t> value)
    {
        _schema->minItems = value;
        return *this;
    }

    JsonFieldBuilder maxItems(std::optional<std::size_t> value)
    {
        _schema->maxItems = value;
        return *this;
    }

    JsonFieldBuilder items()
    {
        assert(!_schema->items.empty());
        return JsonFieldBuilder(_schema->items[0]);
    }

    template<ReflectedJson T>
    JsonFieldBuilder defaultValue(const T &value)
    {
        _schema->defaultValue = serializeToJson(value);
        _schema->required = false;
        return *this;
    }

    JsonFieldBuilder defaultValue(const char *value)
    {
        return defaultValue(std::string(value));
    }

private:
    JsonSchema *_schema;
};

template<typename GetterType, typename ObjectType>
using GetFieldType = std::remove_pointer_t<std::invoke_result_t<GetterType, ObjectType &>>;

template<typename GetterType, typename ObjectType>
concept JsonFieldGetter = std::invocable<GetterType, ObjectType &> && std::invocable<GetterType, const ObjectType &>
    && std::is_pointer_v<std::invoke_result_t<GetterType, ObjectType &>>
    && std::is_pointer_v<std::invoke_result_t<GetterType, const ObjectType &>>
    && (!std::is_const_v<GetFieldType<GetterType, ObjectType>>) && ReflectedJson<GetFieldType<GetterType, ObjectType>>
    && ReflectedJson<std::remove_const_t<GetFieldType<GetterType, const ObjectType>>>;

template<typename T>
class JsonBuilder
{
public:
    void description(std::string value)
    {
        _description = std::move(value);
    }

    template<JsonFieldGetter<T> U>
    JsonFieldBuilder field(std::string name, U getFieldPtr)
    {
        using FieldType = GetFieldType<U, T>;

        auto &field = _fields.emplace_back();

        field.name = std::move(name);

        field.schema = getJsonSchema<FieldType>();

        field.serialize = [=](const auto &object)
        {
            const auto &value = *getFieldPtr(object);
            return serializeToJson(value);
        };

        field.deserialize = [=](const auto &json, auto &object)
        {
            auto &value = *getFieldPtr(object);
            value = deserializeAs<FieldType>(json);
        };

        return JsonFieldBuilder(field.schema);
    }

    JsonFieldBuilder constant(std::string name, const std::string &value)
    {
        auto &field = _fields.emplace_back();

        field.name = std::move(name);

        field.schema = getJsonSchema<std::string>();

        field.schema.constant = value;

        field.serialize = [=](const auto &) { return value; };

        field.deserialize = [=](const auto &json, auto &)
        {
            auto item = deserializeAs<std::string>(json);
            if (item != value)
            {
                throw JsonException("Invalid const");
            }
        };

        return JsonFieldBuilder(field.schema);
    }

    JsonObjectInfo<T> build()
    {
        return JsonObjectInfo<T>(std::exchange(_fields, {}), std::exchange(_description, {}));
    }

private:
    std::vector<JsonField<T>> _fields;
    std::string _description;
};
}

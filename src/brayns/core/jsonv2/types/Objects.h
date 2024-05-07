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

#include <brayns/core/jsonv2/JsonReflector.h>

namespace brayns::experimental
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
    explicit JsonObjectInfo(std::vector<JsonField<T>> fields):
        _fields(std::move(fields))
    {
    }

    JsonSchema getSchema() const
    {
        auto schema = JsonSchema{.type = JsonType::Object};
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
};

template<typename T>
struct JsonObjectReflector;

template<typename T>
concept ReflectedJsonObject = std::same_as<decltype(JsonObjectReflector<T>::reflect()), JsonObjectInfo<T>>;

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

template<typename ParentType>
class JsonFieldBuilder
{
public:
    explicit JsonFieldBuilder(JsonField<ParentType> &field):
        _field(&field)
    {
    }

    JsonFieldBuilder description(std::string value)
    {
        _field->schema.description = std::move(value);
        return *this;
    }

    JsonFieldBuilder required(bool value)
    {
        _field->schema.required = value;
        return *this;
    }

    JsonFieldBuilder minimum(std::optional<double> value)
    {
        _field->schema.minimum = value;
        return *this;
    }

    JsonFieldBuilder maximum(std::optional<double> value)
    {
        _field->schema.maximum = value;
        return *this;
    }

    JsonFieldBuilder minItems(std::optional<std::size_t> value)
    {
        _field->schema.minItems = value;
        return *this;
    }

    JsonFieldBuilder maxItems(std::optional<std::size_t> value)
    {
        _field->schema.maxItems = value;
        return *this;
    }

    template<typename T>
    JsonFieldBuilder defaultValue(const T &value)
    {
        _field->schema.defaultValue = serializeToJson(value);
        _field->schema.required = false;
        return *this;
    }

    JsonFieldBuilder defaultValue(const char *value)
    {
        return defaultValue(std::string(value));
    }

private:
    JsonField<ParentType> *_field;
};

template<typename T>
class JsonObjectInfoBuilder
{
public:
    JsonFieldBuilder<T> field(std::string name, auto &&getPtr)
    {
        using FieldPtr = decltype(getPtr(std::declval<T &>()));

        static_assert(std::is_pointer_v<FieldPtr>, "getPtr must return a pointer to the object field");

        using FieldType = std::decay_t<std::remove_pointer_t<FieldPtr>>;

        auto &field = _fields.emplace_back();

        field.name = std::move(name);
        field.schema = getJsonSchema<FieldType>();
        field.serialize = [=](const auto &object) { return serializeToJson(*getPtr(object)); };
        field.deserialize = [=](const auto &json, auto &object) { *getPtr(object) = deserializeAs<FieldType>(json); };

        return JsonFieldBuilder<T>(field);
    }

    JsonFieldBuilder<T> constant(std::string name, const std::string &value)
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

        return JsonFieldBuilder<T>(field);
    }

    JsonObjectInfo<T> build()
    {
        return JsonObjectInfo<T>(std::exchange(_fields, {}));
    }

private:
    std::vector<JsonField<T>> _fields;
};
}

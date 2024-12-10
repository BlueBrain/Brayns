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

#include "Primitives.h"

namespace brayns
{
template<typename Parent>
struct JsonField
{
    std::string name;
    JsonSchema schema;
    std::function<void(const Parent &, JsonValue &)> serialize;
    std::function<void(const JsonValue &, Parent &)> deserialize;
};

template<typename T>
struct JsonObjectInfo
{
    std::string description;
    std::vector<JsonField<T>> fields;
};

template<typename T>
struct JsonObjectReflector;

template<typename T>
concept ReflectedJsonObject = requires {
    { JsonObjectReflector<T>::reflect() } -> std::same_as<JsonObjectInfo<T>>;
};

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
        return getJsonObjectSchema(info);
    }

    static void serialize(const T &value, JsonValue &json)
    {
        const auto &info = reflectJsonObject<T>();
        serializeJsonObject(info, value, json);
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        const auto &info = reflectJsonObject<T>();
        deserializeJsonObject(info, json, value);
    }
};

template<typename T>
JsonSchema getJsonObjectSchema(const JsonObjectInfo<T> &info)
{
    auto schema = JsonSchema{
        .description = info.description,
        .type = JsonType::Object,
    };

    for (const auto &field : info.fields)
    {
        schema.properties[field.name] = field.schema;
    }

    return schema;
}

template<typename T>
void serializeJsonObject(const JsonObjectInfo<T> &info, const T &value, JsonValue &json)
{
    auto object = createJsonObject();

    for (const auto &field : info.fields)
    {
        auto jsonItem = JsonValue();
        field.serialize(value, jsonItem);
        object->set(field.name, jsonItem);
    }

    json = object;
}

template<typename T>
void deserializeJsonObject(const JsonObjectInfo<T> &info, const JsonValue &json, T &value)
{
    const auto &object = getObject(json);

    for (const auto &field : info.fields)
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

        if (field.schema.defaultValue)
        {
            field.deserialize(*field.schema.defaultValue, value);
        }
    }
}

template<typename T>
concept PtrToReflectedJson = std::is_pointer_v<T> && ReflectedJson<std::decay_t<std::remove_pointer_t<T>>>;

template<typename T>
concept PtrToMutableReflectedJson = PtrToReflectedJson<T> && !std::is_const_v<std::remove_pointer_t<T>>;

template<typename T, typename Parent>
concept JsonFieldGetter = requires(T getter, Parent &object) {
    { getter(std::as_const(object)) } -> PtrToReflectedJson;
    { getter(object) } -> PtrToMutableReflectedJson;
};

template<typename Parent>
JsonField<Parent> createJsonField(std::string name, JsonFieldGetter<Parent> auto getFieldPtr)
{
    using Child = std::decay_t<decltype(*getFieldPtr(std::declval<Parent &>()))>;

    auto serializeField = [=](const auto &object, auto &json)
    {
        const auto &value = *getFieldPtr(object);
        serializeToJson(value, json);
    };

    auto deserializeField = [=](const auto &json, auto &object)
    {
        auto &value = *getFieldPtr(object);
        deserializeJson(json, value);
    };

    return {
        .name = std::move(name),
        .schema = getJsonSchema<Child>(),
        .serialize = std::move(serializeField),
        .deserialize = std::move(deserializeField),
    };
}

template<typename Parent>
JsonField<Parent> createConstJsonField(std::string name, std::string value)
{
    return {
        .name = std::move(name),
        .schema = createJsonConstSchema(value),
        .serialize = [=](const auto &, auto &json) { serializeToJson(value, json); },
        .deserialize = [=](const auto &json, auto &) { deserializeJsonConst(json, value); },
    };
}

template<typename T>
concept PtrToReflectedJsonObject = std::is_pointer_v<T> && ReflectedJsonObject<std::decay_t<std::remove_pointer_t<T>>>;

template<typename T>
concept PtrToMutableReflectedJsonObject = PtrToReflectedJsonObject<T> && !std::is_const_v<std::remove_pointer_t<T>>;

template<typename T, typename Parent>
concept JsonChildGetter = requires(T callable, Parent parent) {
    { callable(std::as_const(parent)) } -> PtrToReflectedJsonObject;
    { callable(parent) } -> PtrToMutableReflectedJsonObject;
};

template<typename Parent, typename Child>
JsonField<Parent> convertJsonField(const JsonField<Child> &child, JsonChildGetter<Parent> auto getChildPtr)
{
    auto serializeChild = child.serialize;
    auto serializeParent = [=](const auto &object, auto &json)
    {
        const auto &childObject = *getChildPtr(object);
        serializeChild(childObject, json);
    };

    auto deserializeChild = child.deserialize;
    auto deserializeParent = [=](const auto &json, auto &object)
    {
        auto &childObject = *getChildPtr(object);
        deserializeChild(json, childObject);
    };

    return {
        .name = child.name,
        .schema = child.schema,
        .serialize = std::move(serializeParent),
        .deserialize = std::move(deserializeParent),
    };
}

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

template<typename T>
class JsonBuilder
{
public:
    void description(std::string value)
    {
        _info.description = std::move(value);
    }

    JsonFieldBuilder constant(std::string name, std::string value)
    {
        auto field = createConstJsonField<T>(std::move(name), std::move(value));
        _info.fields.push_back(std::move(field));
        return JsonFieldBuilder(_info.fields.back().schema);
    }

    JsonFieldBuilder field(std::string name, JsonFieldGetter<T> auto getFieldPtr)
    {
        auto field = createJsonField<T>(std::move(name), std::move(getFieldPtr));
        _info.fields.push_back(std::move(field));
        return JsonFieldBuilder(_info.fields.back().schema);
    }

    void extend(JsonChildGetter<T> auto getChildPtr)
    {
        using Child = std::decay_t<decltype(*getChildPtr(std::declval<T &>()))>;

        const auto &child = reflectJsonObject<Child>();

        for (const auto &field : child.fields)
        {
            auto parentField = convertJsonField<T>(field, std::move(getChildPtr));
            _info.fields.push_back(std::move(parentField));
        }
    }

    JsonObjectInfo<T> build()
    {
        return std::exchange(_info, {});
    }

private:
    JsonObjectInfo<T> _info;
};
}

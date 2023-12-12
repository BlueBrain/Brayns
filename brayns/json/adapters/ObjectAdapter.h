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

#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Reflected info about a JSON object property.
 *
 */
struct JsonProperty
{
    std::string name;
    JsonSchema schema;
    std::function<void(const void *, JsonValue &)> serialize;
    std::function<void(const JsonValue &, void *)> deserialize;
};

/**
 * @brief Reflected info about a JSON object.
 *
 */
struct JsonObjectInfo
{
    std::string title;
    std::vector<JsonProperty> properties;
};

/**
 * @brief Helper to build JSON from reflection info.
 *
 */
class JsonObjectHandler
{
public:
    static JsonSchema getSchema(const JsonObjectInfo &object);
    static void serialize(const JsonObjectInfo &object, const void *value, JsonValue &json);
    static void deserialize(const JsonObjectInfo &object, const JsonValue &json, void *value);
};

class JsonPropertyBuilder
{
public:
    explicit JsonPropertyBuilder(JsonProperty &property);

    JsonPropertyBuilder description(std::string value);
    JsonPropertyBuilder required(bool value);
    JsonPropertyBuilder readOnly(bool value);
    JsonPropertyBuilder writeOnly(bool value);
    JsonPropertyBuilder minimum(double value);
    JsonPropertyBuilder maximum(double value);
    JsonPropertyBuilder minItems(size_t value);
    JsonPropertyBuilder maxItems(size_t value);

    template<typename T>
    JsonPropertyBuilder defaultValue(const T &value)
    {
        JsonAdapter<T>::serialize(value, _property.schema.defaultValue);
        _property.schema.required = false;
        return *this;
    }

    JsonPropertyBuilder defaultValue(const char *value)
    {
        return defaultValue(std::string(value));
    }

private:
    JsonProperty &_property;
};

class JsonReflector
{
public:
    template<typename ObjectType, typename GetterType>
    using Reflect = decltype(std::declval<GetterType>()(std::declval<const ObjectType>()));

    template<typename ObjectType, typename GetterType>
    using Decay = std::decay_t<Reflect<ObjectType, GetterType>>;

    template<typename ObjectType, typename GetterType, typename SetterType>
    static void reflectGetSet(GetterType get, SetterType set, JsonProperty &property)
    {
        using PropertyType = Decay<ObjectType, GetterType>;
        property.schema = JsonAdapter<PropertyType>::getSchema();
        _reflectGet<ObjectType, PropertyType>(get, property);
        _reflectSet<ObjectType, PropertyType>(set, property);
    }

    template<typename ObjectType, typename GetterType>
    static void reflectGet(GetterType get, JsonProperty &property)
    {
        using PropertyType = Decay<ObjectType, GetterType>;
        property.schema = JsonAdapter<PropertyType>::getSchema();
        _reflectGet<ObjectType, PropertyType>(get, property);
    }

    template<typename ObjectType, typename PropertyType, typename SetterType>
    static void reflectSet(SetterType set, JsonProperty &property)
    {
        property.schema = JsonAdapter<PropertyType>::getSchema();
        _reflectSet<ObjectType, PropertyType>(set, property);
    }

private:
    template<typename ObjectType, typename PropertyType, typename GetterType>
    static void _reflectGet(GetterType get, JsonProperty &property)
    {
        property.serialize = [=](const void *ptr, JsonValue &json)
        {
            auto &object = *static_cast<const ObjectType *>(ptr);
            JsonAdapter<PropertyType>::serialize(get(object), json);
        };
    }

    template<typename ObjectType, typename PropertyType, typename SetterType>
    static void _reflectSet(SetterType set, JsonProperty &property)
    {
        property.deserialize = [=](const JsonValue &json, void *ptr)
        {
            auto &object = *static_cast<ObjectType *>(ptr);
            auto value = PropertyType();
            JsonAdapter<PropertyType>::deserialize(json, value);
            set(object, std::move(value));
        };
    }
};

template<typename T>
class JsonObjectBuilder
{
public:
    explicit JsonObjectBuilder(std::string title)
    {
        _object.title = std::move(title);
    }

    JsonObjectInfo build()
    {
        return std::exchange(_object, {});
    }

    template<typename GetterType, typename SetterType>
    JsonPropertyBuilder getset(std::string name, GetterType get, SetterType set)
    {
        auto &property = _emplace(std::move(name));
        JsonReflector::reflectGetSet<T>(get, set, property);
        property.schema.required = true;
        return JsonPropertyBuilder(property);
    }

    template<typename GetterType>
    JsonPropertyBuilder get(std::string name, GetterType get)
    {
        auto &property = _emplace(std::move(name));
        JsonReflector::reflectGet<T>(get, property);
        property.schema.required = true;
        property.schema.readOnly = true;
        return JsonPropertyBuilder(property);
    }

    template<typename PropertyType, typename SetterType>
    JsonPropertyBuilder set(std::string name, SetterType set)
    {
        auto &property = _emplace(std::move(name));
        JsonReflector::reflectSet<T, PropertyType>(set, property);
        property.schema.required = true;
        property.schema.writeOnly = true;
        return JsonPropertyBuilder(property);
    }

private:
    JsonObjectInfo _object;

    JsonProperty &_emplace(std::string name)
    {
        auto &property = _object.properties.emplace_back();
        property.name = std::move(name);
        return property;
    }
};

template<typename T>
struct ObjectAdapter
{
    static JsonSchema getSchema()
    {
        auto &object = _getObject();
        return JsonObjectHandler::getSchema(object);
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto &object = _getObject();
        JsonObjectHandler::serialize(object, &value, json);
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        auto &object = _getObject();
        JsonObjectHandler::deserialize(object, json, &value);
    }

protected:
    using Builder = JsonObjectBuilder<T>;

private:
    static const JsonObjectInfo &_getObject()
    {
        static const JsonObjectInfo object = JsonAdapter<T>::reflect();
        return object;
    }
};
} // namespace brayns

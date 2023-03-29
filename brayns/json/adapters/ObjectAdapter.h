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

#include <brayns/json/JsonAdapter.h>

namespace brayns
{
struct JsonProperty
{
    std::string name;
    JsonSchema schema = JsonSchema::from(NullSchema());
    bool required = true;
    std::function<void(const void *, JsonValue &)> serialize;
    std::function<void(const JsonValue &, void *)> deserialize;
};

struct JsonObjectInfo
{
    std::string title;
    std::vector<JsonProperty> properties;
};

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
    JsonPropertyBuilder defaultValue(const JsonValue &value);
    JsonPropertyBuilder minimum(double value);
    JsonPropertyBuilder maximum(double value);
    JsonPropertyBuilder minItems(size_t value);
    JsonPropertyBuilder maxItems(size_t value);
    JsonPropertyBuilder required(bool value);

private:
    JsonProperty &_property;
};

template<typename ObjectType>
class JsonReflector
{
public:
    template<typename GetterType>
    using Reflect = decltype(std::declval<GetterType>()(std::declval<ObjectType>()));

    template<typename GetterType>
    using Decay = std::decay_t<Reflect<ObjectType, GetterType>>;

    template<typename GetterType, typename SetterType>
    static void reflectGetSet(GetterType get, SetterType set, JsonProperty &property)
    {
        using PropertyType = Decay<ObjectType, GetterType>;
        property.schema = JsonAdapter<PropertyType>::getSchema();
        _reflectGet<ObjectType, PropertyType>(get, property);
        _reflectSet<ObjectType, PropertyType>(set, property);
    }

    template<typename GetterType>
    static void reflectGet(GetterType get, JsonProperty &property)
    {
        using PropertyType = Decay<ObjectType, GetterType>;
        property.schema = JsonAdapter<PropertyType>::getSchema();
        _reflectGet<ObjectType, PropertyType>(get, property);
    }

    template<typename PropertyType, typename SetterType>
    static void reflectSet(SetterType set, JsonProperty &property)
    {
        property.schema = JsonAdapter<PropertyType>::getSchema();
        _reflectSet<ObjectType, PropertyType>(set, property);
    }

private:
    template<typename PropertyType, typename GetterType>
    static void _reflectGet(GetterType get, JsonProperty &property)
    {
        property.serialize = [=](const void *ptr, JsonValue &json)
        {
            auto &object = *static_cast<const ObjectType *>(ptr);
            JsonAdapter<PropertyType>::serialize(get(object), json);
        };
    };

    template<typename PropertyType, typename SetterType>
    static void _reflectSet(SetterType set, JsonProperty &property)
    {
        property.deserialize = [=](const JsonValue &json, void *ptr)
        {
            auto &object = *static_cast<ObjectType *>(ptr);
            auto value = PropertyType();
            JsonAdapter<PropertyType>::deserialize(json, value);
            set(object, std::move(value));
        };
    };
};

template<typename ObjectType>
class JsonObjectBuilder
{
public:
    explicit JsonObjectBuilder(JsonObjectInfo &object)
        : _object(object)
    {
    }

    JsonObjectBuilder title(std::string value)
    {
        _object.title = std::move(value);
        return *this;
    }

    template<typename GetterType, typename SetterType>
    JsonPropertyBuilder property(std::string name, GetterType get, SetterType set)
    {
        auto &property = _object.properties.emplace_back();
        property.name = std::move(name);
        JsonReflector<ObjectType>::reflectGetSet(get, set, property);
        return JsonPropertyBuilder(property);
    }

    template<typename GetterType>
    JsonPropertyBuilder readOnly(std::string name, GetterType get)
    {
        auto &property = _object.properties.emplace_back();
        property.name = std::move(name);
        JsonReflector<ObjectType>::reflectGet(get, property);
        property.schema.options.readOnly = true;
        return JsonPropertyBuilder(property);
    }

    template<typename PropertyType, typename SetterType>
    JsonPropertyBuilder writeOnly(std::string name, SetterType set)
    {
        auto &property = _object.properties.emplace_back();
        property.name = std::move(name);
        JsonReflector<ObjectType>::reflectSet<PropertyType>(set, property);
        property.schema.options.writeOnly = true;
        return JsonPropertyBuilder(property);
    }

private:
    JsonObjectInfo &_object;
};

template<typename T>
struct ObjectAdapter
{
    static JsonSchema getSchema()
    {
        return JsonObjectHandler::getSchema(_object);
    }

    static void serialize(const T &value, JsonValue &json)
    {
        JsonObjectHandler::serialize(_object, &value, json);
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        JsonObjectHandler::deserialize(_object, json, &value);
    }

private:
    static JsonObjectInfo _buildObject()
    {
        auto object = JsonObjectInfo();
        auto builder = JsonObjectBuilder<T>(object);
        JsonAdapter<T>::reflect(builder);
        return object;
    }

    static inline JsonObjectInfo _object = _buildObject();
};
} // namespace brayns

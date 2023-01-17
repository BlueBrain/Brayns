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

#include <brayns/utils/FunctorInfo.h>

#include "JsonObjectInfo.h"

/**
 * @brief Create a JsonAdapter for the given type and name.
 *
 * The name is a string identifying the type (schema title).
 *
 * Declaring a JsonAdapter allow the Json class to build schema, serialize and
 * deserialize the given type using JSON.
 *
 * All JSON adapters (defined with macro or not) must be in brayns namespace to
 * be used by brayns JSON functionalities (Json class).
 *
 * Usage:
 * @code {.cpp}
 * BRAYNS_JSON_ADAPTER_BEGIN(MyType)
 * BRAYNS_JSON_ADAPTER_GETSET("a", getA, setA, "A get-set property")
 * BRAYNS_JSON_ADAPTER_GET("b", getB, "A read only property")
 * BRAYNS_JSON_ADAPTER_SET("c", setC, "A write only property")
 * BRAYNS_JSON_ADAPTER_ENTRY(d, "A public field")
 * BRAYNS_JSON_ADAPTER_NAMED_ENTRY("test", e, "A public field with given name")
 * BRAYNS_JSON_ADAPTER_END()
 * @endcode
 *
 */
#define BRAYNS_NAMED_JSON_ADAPTER_BEGIN(TYPE, NAME) \
    template<> \
    struct JsonAdapter<TYPE> \
    { \
    public: \
        static JsonSchema getSchema() \
        { \
            return _info.getSchema(); \
        } \
\
        static void serialize(const TYPE &value, JsonValue &json) \
        { \
            _info.serialize(&value, json); \
        } \
\
        static void deserialize(const JsonValue &json, TYPE &value) \
        { \
            _info.deserialize(json, &value); \
        } \
\
    private: \
        using _ObjectType = TYPE; \
\
        static inline const JsonObjectInfo _info = [] { \
            JsonObjectInfo _result(NAME);
/**
 * @brief Shortcut to have the same type symbol and name.
 *
 */
#define BRAYNS_JSON_ADAPTER_BEGIN(TYPE) BRAYNS_NAMED_JSON_ADAPTER_BEGIN(TYPE, #TYPE)

/**
 * @brief Register a property in the current adapter.
 *
 * Parameters are the property name, a functor to get its schema, serialize and
 * deserialize the property.
 *
 */
#define BRAYNS_JSON_ADAPTER_PROPERTY(NAME, SCHEMA, TOJSON, FROMJSON, ...) \
    { \
        JsonObjectProperty _property; \
        _property.name = NAME; \
        _property.options = JsonOptions{__VA_ARGS__}; \
        _property.getSchema = SCHEMA; \
        _property.serialize = [](const void *data, JsonValue &json) \
        { \
            auto &object = *static_cast<const _ObjectType *>(data); \
            TOJSON(object, json); \
        }; \
        _property.deserialize = [](const JsonValue &json, void *data) \
        { \
            auto &object = *static_cast<_ObjectType *>(data); \
            FROMJSON(json, object); \
        }; \
        _result.addProperty(std::move(_property)); \
    }

/**
 * @brief Shortcut to create a functor to get the schema of an object using Json
 * class.
 *
 * The argument is a functor returning the property from a TYPE instance.
 *
 */
#define BRAYNS_JSON_ADAPTER_SCHEMA(GET) \
    [] \
    { \
        using GetterType = decltype(&_ObjectType::GET); \
        using ReturnType = DecayReturnType<GetterType>; \
        return Json::getSchema<ReturnType>(); \
    }

/**
 * @brief Shortcut to generate a JSON schema from a setter.
 *
 * setStuff(const T &) -> Json::getSchema<T>().
 *
 */
#define BRAYNS_JSON_ADAPTER_DEFAULT_SCHEMA(SET) \
    [] \
    { \
        using SetterType = decltype(&_ObjectType::SET); \
        using ArgType = DecayFirstArgType<SetterType>; \
        return Json::getSchema<ArgType>(); \
    }

/**
 * @brief Shortcut to create a functor to serialize an object using Json class.
 *
 * The argument is a functor returning the property from a TYPE instance.
 *
 */
#define BRAYNS_JSON_ADAPTER_TOJSON(GET) [](const auto &object, auto &json) { Json::serialize(object.GET(), json); }

/**
 * @brief Shortcut to create a functor to serialize an object using Json class.
 *
 * The arguments are object setter name and a default factory.
 *
 */
#define BRAYNS_JSON_ADAPTER_FROMJSON(SET, DEFAULT) \
    [](const auto &json, auto &object) \
    { \
        auto buffer = DEFAULT(object); \
        Json::deserialize(json, buffer); \
        object.SET(std::move(buffer)); \
    }

/**
 * @brief Shortcut for a deserializer using getter and setter.
 *
 */
#define BRAYNS_JSON_ADAPTER_GETSET_FROMJSON(GET, SET) \
    BRAYNS_JSON_ADAPTER_FROMJSON(SET, [](auto &object) { return object.GET(); })

/**
 * @brief Shortcut for a deserializer using only setter.
 *
 */
#define BRAYNS_JSON_ADAPTER_SET_FROMJSON(SET) \
    BRAYNS_JSON_ADAPTER_FROMJSON( \
        SET, \
        [](const auto &) \
        { \
            using SetterType = decltype(&_ObjectType::SET); \
            using ArgType = DecayFirstArgType<SetterType>; \
            return ArgType{}; \
        })

/**
 * @brief Register a property that can be get and set.
 *
 * The arguments are the property name, its getter and setter and a description.
 *
 */
#define BRAYNS_JSON_ADAPTER_GETSET(NAME, GET, SET, DESCRIPTION, ...) \
    BRAYNS_JSON_ADAPTER_PROPERTY( \
        NAME, \
        BRAYNS_JSON_ADAPTER_SCHEMA(GET), \
        BRAYNS_JSON_ADAPTER_TOJSON(GET), \
        BRAYNS_JSON_ADAPTER_GETSET_FROMJSON(GET, SET), \
        Description(DESCRIPTION), \
        __VA_ARGS__)

/**
 * @brief Register a property that can only be get.
 *
 */
#define BRAYNS_JSON_ADAPTER_GET(NAME, GET, DESCRIPTION, ...) \
    BRAYNS_JSON_ADAPTER_PROPERTY( \
        NAME, \
        BRAYNS_JSON_ADAPTER_SCHEMA(GET), \
        BRAYNS_JSON_ADAPTER_TOJSON(GET), \
        [](const auto &, auto &) {}, \
        Description(DESCRIPTION), \
        ReadOnly(), \
        __VA_ARGS__)

/**
 * @brief Register a property that can only be set.
 *
 */
#define BRAYNS_JSON_ADAPTER_SET(NAME, SET, DESCRIPTION, ...) \
    BRAYNS_JSON_ADAPTER_PROPERTY( \
        NAME, \
        BRAYNS_JSON_ADAPTER_DEFAULT_SCHEMA(SET), \
        [](const auto &, auto &) {}, \
        BRAYNS_JSON_ADAPTER_SET_FROMJSON(SET), \
        Description(DESCRIPTION), \
        WriteOnly(), \
        __VA_ARGS__)

/**
 * @brief Register a public property.
 *
 */
#define BRAYNS_JSON_ADAPTER_FIELD(NAME, FIELD, ...) \
    BRAYNS_JSON_ADAPTER_PROPERTY( \
        NAME, \
        [] { return Json::getSchema<decltype(_ObjectType::FIELD)>(); }, \
        [](const auto &object, auto &json) { Json::serialize(object.FIELD, json); }, \
        [](const auto &json, auto &object) { Json::deserialize(json, object.FIELD); }, \
        __VA_ARGS__)

/**
 * @brief Register a public mandatory property with custom name and description.
 *
 */
#define BRAYNS_JSON_ADAPTER_NAMED_ENTRY(NAME, FIELD, DESCRIPTION, ...) \
    BRAYNS_JSON_ADAPTER_FIELD(NAME, FIELD, Description(DESCRIPTION), Required(), __VA_ARGS__)

/**
 * @brief Register an entry with same name as the field.
 *
 */
#define BRAYNS_JSON_ADAPTER_ENTRY(FIELD, DESCRIPTION, ...) \
    BRAYNS_JSON_ADAPTER_NAMED_ENTRY(#FIELD, FIELD, DESCRIPTION, __VA_ARGS__)

/**
 * @brief To be called after BRAYNS_JSON_ADAPTER_BEGIN() and a set of
 * properties.
 *
 */
#define BRAYNS_JSON_ADAPTER_END() \
    return _result; \
    } \
    (); \
    } \
    ;

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

#include <brayns/common/utils/FunctorInfo.h>

#include "Message.h"

namespace brayns
{
/**
 * @brief Create a JsonAdapter for the given type and name.
 *
 * The name is a string identifying the type (schema title).
 *
 * Declaring a JsonAdapter allow the Json class to build schema, serialize and
 * deserialize the given type using JSON.
 *
 * Usage:
 * @code {.cpp}
 * BRAYNS_ADAPTER_BEGIN(MyType)
 * BRAYNS_ADAPTER_GETSET("a", getA, setA, "A property with getter and setter")
 * BRAYNS_ADAPTER_GET("b", getB, "A read only property with only a getter")
 * BRAYNS_ADAPTER_SET("c", setC, "A write only property with only a setter")
 * BRAYNS_ADAPTER_ENTRY(d, "A public field mapped as property with same name")
 * BRAYNS_ADAPTER_NAMED_ENTRY("test", e, "An entry with specific name")
 * BRAYNS_ADAPTER_END()
 * @endcode
 *
 */
#define BRAYNS_NAMED_ADAPTER_BEGIN(TYPE, NAME)                      \
    template <>                                                     \
    struct JsonAdapter<TYPE>                                        \
    {                                                               \
    public:                                                         \
        static JsonSchema getSchema(const TYPE& value)              \
        {                                                           \
            return _getInfo().getSchema(&value);                    \
        }                                                           \
                                                                    \
        static bool serialize(const TYPE& value, JsonValue& json)   \
        {                                                           \
            return _getInfo().serialize(&value, json);              \
        }                                                           \
                                                                    \
        static bool deserialize(const JsonValue& json, TYPE& value) \
        {                                                           \
            return _getInfo().deserialize(json, &value);            \
        }                                                           \
                                                                    \
    private:                                                        \
        using ObjectType = TYPE;                                    \
                                                                    \
        static MessageInfo& _getInfo()                              \
        {                                                           \
            static MessageInfo info = _createInfo();                \
            return info;                                            \
        }                                                           \
                                                                    \
        static MessageInfo _createInfo()                            \
        {                                                           \
            MessageInfo info(NAME);

/**
 * @brief Shortcut to have the same type symbol and name.
 *
 */
#define BRAYNS_ADAPTER_BEGIN(TYPE) BRAYNS_NAMED_ADAPTER_BEGIN(TYPE, #TYPE)

/**
 * @brief Register a property in the current adapter.
 *
 * Parameters are the property name, a functor to get its schema, serialize and
 * deserialize the property.
 *
 */
#define BRAYNS_ADAPTER_PROPERTY(NAME, SCHEMA, TOJSON, FROMJSON, ...) \
    {                                                                \
        MessageProperty property;                                    \
        property.name = NAME;                                        \
        property.options = {__VA_ARGS__};                            \
        property.getSchema = [](const void* data)                    \
        {                                                            \
            auto& object = *static_cast<const ObjectType*>(data);    \
            return SCHEMA(object);                                   \
        };                                                           \
        property.serialize = [](const void* data, JsonValue& json)   \
        {                                                            \
            auto& object = *static_cast<const ObjectType*>(data);    \
            return TOJSON(object, json);                             \
        };                                                           \
        property.deserialize = [](const JsonValue& json, void* data) \
        {                                                            \
            auto& object = *static_cast<ObjectType*>(data);          \
            return FROMJSON(json, object);                           \
        };                                                           \
        info.addProperty(std::move(property));                       \
    }

/**
 * @brief Shortcut to create a functor to get the schema of an object using Json
 * class.
 *
 * The argument is a functor returning the property from a TYPE instance.
 *
 */
#define BRAYNS_ADAPTER_SCHEMA(GET) \
    [](const auto& object) { return Json::getSchema(object.GET()); }

/**
 * @brief Shortcut to create a functor to serialize an object using Json class.
 *
 * The argument is a functor returning the property from a TYPE instance.
 *
 */
#define BRAYNS_ADAPTER_TOJSON(GET)     \
    [](const auto& object, auto& json) \
    { return Json::serialize(object.GET(), json); }

/**
 * @brief Shortcut to get the decay argument type of an object method.
 *
 * The arguments are an object instance and the method name.
 *
 */
#define BRAYNS_ADAPTER_ARGTYPE(OBJECT, METHOD) \
    DecayArgType<decltype(&std::decay_t<decltype(OBJECT)>::METHOD), 0>

/**
 * @brief Shortcut to create a functor to serialize an object using Json class.
 *
 * The argument is a functor setting the property from a JsonValue and a TYPE
 * instance.
 *
 */
#define BRAYNS_ADAPTER_FROMJSON(SET)                   \
    [](const auto& json, auto& object)                 \
    {                                                  \
        using T = BRAYNS_ADAPTER_ARGTYPE(object, SET); \
        T buffer{};                                    \
        if (!Json::deserialize(json, buffer))          \
        {                                              \
            return false;                              \
        }                                              \
        object.SET(std::move(buffer));                 \
        return true;                                   \
    }

/**
 * @brief Register a property that can be get and set.
 *
 * The arguments are the property name, its getter and setter and a description.
 *
 */
#define BRAYNS_ADAPTER_GETSET(NAME, GET, SET, DESCRIPTION, ...) \
    BRAYNS_ADAPTER_PROPERTY(NAME, BRAYNS_ADAPTER_SCHEMA(GET),   \
                            BRAYNS_ADAPTER_TOJSON(GET),         \
                            BRAYNS_ADAPTER_FROMJSON(SET),       \
                            Description(DESCRIPTION), __VA_ARGS__)

/**
 * @brief Register a property that can only be get.
 *
 */
#define BRAYNS_ADAPTER_GET(NAME, GET, DESCRIPTION, ...)                    \
    BRAYNS_ADAPTER_PROPERTY(                                               \
        NAME, BRAYNS_ADAPTER_SCHEMA(GET), BRAYNS_ADAPTER_TOJSON(GET),      \
        [](const auto&, auto&) { return true; }, Description(DESCRIPTION), \
        ReadOnly(), __VA_ARGS__)

/**
 * @brief Register a property that can only be set.
 *
 */
#define BRAYNS_ADAPTER_SET(NAME, SET, DESCRIPTION, ...)                        \
    BRAYNS_ADAPTER_PROPERTY(                                                   \
        NAME,                                                                  \
        [](const auto& object)                                                 \
        {                                                                      \
            using T = BRAYNS_ADAPTER_ARGTYPE(object, SET);                     \
            return Json::getSchema<T>();                                       \
        },                                                                     \
        [](const auto&, auto&) { return true; }, BRAYNS_ADAPTER_FROMJSON(SET), \
        Description(DESCRIPTION), WriteOnly(), __VA_ARGS__)

/**
 * @brief Register a public property.
 *
 */
#define BRAYNS_ADAPTER_FIELD(NAME, FIELD, ...)                            \
    BRAYNS_ADAPTER_PROPERTY(                                              \
        NAME,                                                             \
        [](const auto& object) { return Json::getSchema(object.FIELD); }, \
        [](const auto& object, auto& json)                                \
        { return Json::serialize(object.FIELD, json); },                  \
        [](const auto& json, auto& object)                                \
        { return Json::deserialize(json, object.FIELD); },                \
        __VA_ARGS__)

/**
 * @brief Register a public mandatory property with custom name and description.
 *
 */
#define BRAYNS_ADAPTER_NAMED_ENTRY(NAME, FIELD, DESCRIPTION, ...)           \
    BRAYNS_ADAPTER_FIELD(NAME, FIELD, Description(DESCRIPTION), Required(), \
                         __VA_ARGS__)

/**
 * @brief Register an entry with same name as the field.
 *
 */
#define BRAYNS_ADAPTER_ENTRY(FIELD, DESCRIPTION, ...) \
    BRAYNS_ADAPTER_NAMED_ENTRY(#FIELD, FIELD, DESCRIPTION, __VA_ARGS__)

/**
 * @brief To be called after BRAYNS_ADAPTER_BEGIN() and a set of properties.
 *
 */
#define BRAYNS_ADAPTER_END() \
    return info;             \
    }                        \
    }                        \
    ;
} // namespace brayns
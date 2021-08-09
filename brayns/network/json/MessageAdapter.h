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

#define BRAYNS_ADAPTER_BEGIN(TYPE) BRAYNS_NAMED_ADAPTER_BEGIN(TYPE, #TYPE)

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

#define BRAYNS_ADAPTER_SCHEMA(GET) \
    [](const auto& object) { return Json::getSchema(object.GET()); }

#define BRAYNS_ADAPTER_TOJSON(GET)     \
    [](const auto& object, auto& json) \
    { return Json::serialize(object.GET(), json); }

#define BRAYNS_ADAPTER_ARGTYPE(OBJECT, METHOD) \
    DecayArgType<decltype(&std::decay_t<decltype(OBJECT)>::METHOD), 0>

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

#define BRAYNS_ADAPTER_GETSET(NAME, GET, SET, DESCRIPTION, ...) \
    BRAYNS_ADAPTER_PROPERTY(NAME, BRAYNS_ADAPTER_SCHEMA(GET),   \
                            BRAYNS_ADAPTER_TOJSON(GET),         \
                            BRAYNS_ADAPTER_FROMJSON(SET),       \
                            Description(DESCRIPTION), __VA_ARGS__)

#define BRAYNS_ADAPTER_GET(NAME, GET, DESCRIPTION, ...)                    \
    BRAYNS_ADAPTER_PROPERTY(                                               \
        NAME, BRAYNS_ADAPTER_SCHEMA(GET), BRAYNS_ADAPTER_TOJSON(GET),      \
        [](const auto&, auto&) { return true; }, Description(DESCRIPTION), \
        ReadOnly(), __VA_ARGS__)

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

#define BRAYNS_ADAPTER_FIELD(NAME, FIELD, ...)                            \
    BRAYNS_ADAPTER_PROPERTY(                                              \
        NAME,                                                             \
        [](const auto& object) { return Json::getSchema(object.FIELD); }, \
        [](const auto& object, auto& json)                                \
        { return Json::serialize(object.FIELD, json); },                  \
        [](const auto& json, auto& object)                                \
        { return Json::deserialize(json, object.FIELD); },                \
        __VA_ARGS__)

#define BRAYNS_ADAPTER_NAMED_ENTRY(NAME, FIELD, DESCRIPTION, ...) \
    BRAYNS_ADAPTER_FIELD(NAME, FIELD, Description(DESCRIPTION),   \
                         Required(), __VA_ARGS__)

#define BRAYNS_ADAPTER_ENTRY(FIELD, DESCRIPTION, ...) \
    BRAYNS_ADAPTER_NAMED_ENTRY(#FIELD, FIELD, DESCRIPTION, __VA_ARGS__)

#define BRAYNS_ADAPTER_END() \
    return info;             \
    }                        \
    }                        \
    ;
} // namespace brayns
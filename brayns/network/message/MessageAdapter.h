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

#define BRAYNS_ADAPTER_PROPERTY(NAME, GET, SET, ...)                 \
    {                                                                \
        MessageProperty property;                                    \
        property.name = NAME;                                        \
        property.options = {__VA_ARGS__};                            \
        property.getSchema = [](const void* data)                    \
        {                                                            \
            auto& object = *static_cast<const ObjectType*>(data);    \
            return Json::getSchema(GET(object));                     \
        };                                                           \
        property.serialize = [](const void* data, JsonValue& json)   \
        {                                                            \
            auto& object = *static_cast<const ObjectType*>(data);    \
            Json::serialize(GET(object), json);                      \
        };                                                           \
        property.deserialize = [](const JsonValue& json, void* data) \
        {                                                            \
            auto& object = *static_cast<ObjectType*>(data);          \
            auto buffer = GET(object);                               \
            if (Json::deserialize(json, buffer))                     \
            {                                                        \
                SET(object, std::move(buffer));                      \
            }                                                        \
        };                                                           \
        info.addProperty(std::move(property));                       \
    };

#define BRAYNS_ADAPTER_GETSET(NAME, GET, SET, DESCRIPTION, ...)            \
    BRAYNS_ADAPTER_PROPERTY(                                               \
        NAME,                                                              \
        [](const auto& object) -> decltype(auto) { return object.GET(); }, \
        [](auto& object, auto&& value)                                     \
        { object.SET(std::forward<decltype(value)>(value)); },             \
        Description(DESCRIPTION), __VA_ARGS__)

#define BRAYNS_ADAPTER_GET(NAME, GET, DESCRIPTION, ...)                    \
    BRAYNS_ADAPTER_PROPERTY(                                               \
        NAME,                                                              \
        [](const auto& object) -> decltype(auto) { return object.GET(); }, \
        [](auto& object, auto&& value) {}, Description(DESCRIPTION),       \
        ReadOnly(), __VA_ARGS__)

#define BRAYNS_ADAPTER_FIELD(NAME, FIELD, ...)                             \
    BRAYNS_ADAPTER_PROPERTY(                                               \
        NAME,                                                              \
        [](const auto& object) -> decltype(auto) { return object.FIELD; }, \
        [](auto& object, auto&& value)                                     \
        { object.FIELD = std::forward<decltype(value)>(value); },          \
        __VA_ARGS__)

#define BRAYNS_ADAPTER_ENTRY(NAME, FIELD, DESCRIPTION, ...) \
    BRAYNS_ADAPTER_FIELD(NAME, FIELD, Description(DESCRIPTION), __VA_ARGS__)

#define BRAYNS_ADAPTER_END() \
    return info;             \
    }                        \
    }                        \
    ;
} // namespace brayns
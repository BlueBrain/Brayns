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

#include <functional>
#include <string>
#include <vector>

#include "Message.h"

namespace brayns
{
template <typename T>
struct MessageAdapter
{
    static_assert(false,
                  "Please specialize MessageAdapter<T> to provide message "
                  "metadata on T")
};

#define BRAYNS_MESSAGE_ADAPTER_BEGIN(TYPE)                          \
    template <>                                                     \
    struct MessageAdapter<TYPE>                                     \
    {                                                               \
        static const JsonSchema& getSchema()                        \
        {                                                           \
            return _getMessageInfo().getSchema();                   \
        }                                                           \
                                                                    \
        static bool serialize(const TYPE& value, JsonValue& json)   \
        {                                                           \
            return _getMessageInfo().serialize(&value, json);       \
        }                                                           \
                                                                    \
        static bool deserialize(const TYPE& json, TYPE& value)      \
        {                                                           \
            return _getMessageInfo().deserialize(json, &value);     \
        }                                                           \
                                                                    \
    private:                                                        \
        static const MessageInfo& _getMessageInfo()                 \
        {                                                           \
            static const MessageInfo info = _createMessageInfo();   \
            return info;                                            \
        }                                                           \
                                                                    \
        static MessageInfo _createMessageInfo();                    \
    };                                                              \
                                                                    \
    template <>                                                     \
    struct JsonSerializer<TYPE>                                     \
    {                                                               \
        static bool serialize(const TYPE& value, JsonValue& json)   \
        {                                                           \
            return MessageAdapter<TYPE>::serialize(value, json);    \
        }                                                           \
                                                                    \
        static bool deserialize(const JsonValue& json, TYPE& value) \
        {                                                           \
            return MessageAdapter<TYPE>::deserialize(json, value);  \
        }                                                           \
    };                                                              \
                                                                    \
    template <>                                                     \
    struct JsonSchemaFactory<TYPE>                                  \
    {                                                               \
        static JsonSchema createSchema()                            \
        {                                                           \
            return MessageAdapter<TYPE>::getSchema();               \
        }                                                           \
    };                                                              \
                                                                    \
    inline MessageInfo MessageAdapter<TYPE>::_createMessageInfo()   \
    {                                                               \
        using MessageType = TYPE;                                   \
        MessageInfo info(#TYPE);

#define BRAYNS_MESSAGE_ADAPTER_ENTRY(TYPE, NAME, DESCRIPTION, GET, SET) \
    {                                                                   \
        MessageProperty property;                                       \
        property.schema = JsonSchemaFactory<TYPE>::createSchema();      \
        property.schema.name = NAME;                                    \
        property.schema.description = DESCRIPTION;                      \
        property.serialize = [](const void* value, JsonValue& json)     \
        {                                                               \
            auto& message = *static_cast<const MessageType*>(value);    \
            Json::serialize(message.GET(), json);                       \
        };                                                              \
        property.deserialize = [](const JsonValue& json, void* value)   \
        {                                                               \
            auto& message = *static_cast<MessageType*>(value);          \
            decltype(message.GET()) buffer;                             \
            if (Json::deserialize(json, buffer))                        \
            {                                                           \
                message.SET(buffer);                                    \
            }                                                           \
        };                                                              \
        info.addProperty(property);                                     \
    }

#define BRAYNS_MESSAGE_ADAPTER_END() \
    return info;                     \
    }
} // namespace brayns
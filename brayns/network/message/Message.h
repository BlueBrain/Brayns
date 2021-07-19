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

#include <brayns/engine/Engine.h>

#include <brayns/pluginapi/PluginAPI.h>

#include <brayns/parameters/ParametersManager.h>

#include "adapters/ArrayAdapter.h"
#include "adapters/EnumAdapter.h"
#include "adapters/GlmAdapter.h"
#include "adapters/JsonSchemaAdapter.h"
#include "adapters/MapAdapter.h"
#include "adapters/PrimitiveAdapter.h"
#include "adapters/PropertyMapAdapter.h"
#include "adapters/PtrAdapter.h"

#include "JsonOptions.h"
#include "JsonType.h"

namespace brayns
{
/**
 * @brief Message property info.
 *
 */
struct MessageProperty
{
    std::string name;
    JsonSchema schema;
    JsonOptions options;
    std::function<void(const void*, JsonValue&)> serialize;
    std::function<void(const JsonValue&, void*)> deserialize;
};

/**
 * @brief Message metadata.
 *
 */
class MessageInfo
{
public:
    MessageInfo(const std::string& title)
    {
        _schema.title = title;
        _schema.type = JsonType::Object;
    }

    const JsonSchema& getSchema() const { return _schema; }

    bool serialize(const void* message, JsonValue& json) const
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& property : _properties)
        {
            JsonValue jsonProperty;
            property.serialize(message, jsonProperty);
            object->set(property.name, jsonProperty);
        }
        json = object;
        return true;
    }

    bool deserialize(const JsonValue& json, void* message) const
    {
        auto object = JsonHelper::extractObject(json);
        if (!object)
        {
            return false;
        }
        for (const auto& property : _properties)
        {
            auto jsonProperty = object->get(property.name);
            property.deserialize(jsonProperty, message);
        }
        return true;
    }

    void addProperty(const MessageProperty& property)
    {
        auto& name = property.name;
        auto& schema = _schema.properties[name];
        schema = property.schema;
        auto& options = property.options;
        JsonSchemaHelper::setOptions(schema, options);
        if (options.required)
        {
            _schema.required.push_back(name);
        }
        _properties.push_back(property);
    }

private:
    JsonSchema _schema;
    std::vector<MessageProperty> _properties;
};

/**
 * @brief Macro to declare a new message.
 *
 * The resulting message will have the symbol declared in MESSAGE_BEGIN and can
 * be used in JSON serialization with no additional code. A static instance of
 * MessageInfo will be stored inside the resulting message type and can be
 * retreived with the static method getMessageInfo().
 *
 * Example:
 * @code {.cpp}
 * // Declaration
 * BRAYNS_MESSAGE_BEGIN(MyMessage)
 * BRAYNS_MESSAGE_ENTRY(int, anEntry, "This is an entry")
 * BRAYNS_MESSAGE_ENTRY(std::vector<std::string>, someEntries, "Some entries")
 * BRAYNS_MESSAGE_END()
 *
 * // Usage
 * std::string json = Json::stringify(MyMessage());
 * MyMessage message = Json::parse<MyMessage>(json);
 * @endcode
 *
 */
#define BRAYNS_NAMED_MESSAGE_BEGIN(TYPE, NAME)                           \
    struct TYPE                                                          \
    {                                                                    \
    private:                                                             \
        using MessageType = TYPE;                                        \
                                                                         \
        static MessageInfo& _getInfo()                                   \
        {                                                                \
            static MessageInfo info(NAME);                               \
            return info;                                                 \
        }                                                                \
                                                                         \
        static const MessageInfo& _loadInfo()                            \
        {                                                                \
            static const int buildMessageInfo = []                       \
            {                                                            \
                TYPE();                                                  \
                return 0;                                                \
            }();                                                         \
            return _getInfo();                                           \
        }                                                                \
                                                                         \
    public:                                                              \
        TYPE() = default;                                                \
                                                                         \
        JsonSchema getSchema() const { return _loadInfo().getSchema(); } \
                                                                         \
        bool serialize(JsonValue& json) const                            \
        {                                                                \
            return _loadInfo().serialize(this, json);                    \
        }                                                                \
                                                                         \
        bool deserialize(const JsonValue& json)                          \
        {                                                                \
            return _loadInfo().deserialize(json, this);                  \
        }

#define BRAYNS_MESSAGE_BEGIN(TYPE) BRAYNS_NAMED_MESSAGE_BEGIN(TYPE, #TYPE)

/**
 * @brief Add an entry to the current message.
 *
 * Must be called only after BRAYNS_BEGIN_MESSAGE(...). The active message will
 * have a public attribute called NAME of type TYPE, the given description and
 * will be serialized using JsonAdapter<TYPE>.
 *
 */
#define BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, ...)                          \
    TYPE NAME = []                                                        \
    {                                                                     \
        static const int registerEntry = []                               \
        {                                                                 \
            MessageProperty property;                                     \
            property.name = #NAME;                                        \
            property.schema = Json::getSchema<TYPE>();                    \
            property.options = {__VA_ARGS__};                             \
            property.serialize = [](const void* value, JsonValue& json)   \
            {                                                             \
                auto& message = *static_cast<const MessageType*>(value);  \
                Json::serialize(message.NAME, json);                      \
            };                                                            \
            property.deserialize = [](const JsonValue& json, void* value) \
            {                                                             \
                auto& message = *static_cast<MessageType*>(value);        \
                Json::deserialize(json, message.NAME);                    \
            };                                                            \
            _getInfo().addProperty(std::move(property));                  \
            return 0;                                                     \
        }();                                                              \
        return TYPE{};                                                    \
    }();

#define BRAYNS_MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION) \
    BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, Description(DESCRIPTION), Required())

#define BRAYNS_MESSAGE_OPTION(TYPE, NAME, DESCRIPTION) \
    BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, Description(DESCRIPTION))

/**
 * @brief Must be called after BRAYNS_MESSAGE_BEGIN and a set of
 * BRAYNS_MESSAGE_ENTRY to generate valid code.
 *
 */
#define BRAYNS_MESSAGE_END() \
    }                        \
    ;
} // namespace brayns
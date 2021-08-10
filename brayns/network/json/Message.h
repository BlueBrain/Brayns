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

#include <boost/optional.hpp>

#include <brayns/engine/Engine.h>

#include <brayns/pluginapi/PluginAPI.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/network/adapters/ArrayAdapter.h>
#include <brayns/network/adapters/EnumAdapter.h>
#include <brayns/network/adapters/GlmAdapter.h>
#include <brayns/network/adapters/JsonSchemaAdapter.h>
#include <brayns/network/adapters/MapAdapter.h>
#include <brayns/network/adapters/PrimitiveAdapter.h>
#include <brayns/network/adapters/PropertyMapAdapter.h>
#include <brayns/network/adapters/PtrAdapter.h>

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
    JsonOptions options;
    std::function<JsonSchema(const void*)> getSchema;
    std::function<bool(const void*, JsonValue&)> serialize;
    std::function<bool(const JsonValue&, void*)> deserialize;

    JsonSchema getSchemaWithOptions(const void* message) const
    {
        auto schema = getSchema(message);
        JsonSchemaOptions::add(schema, options);
        return schema;
    }

    void add(JsonSchema& schema, const void* message) const
    {
        if (isRequired())
        {
            auto& required = schema.required;
            required.push_back(name);
        }
        schema.properties[name] = getSchemaWithOptions(message);
    }

    JsonValue extract(const JsonObject& object) const
    {
        auto json = object.get(name);
        if (!json.isEmpty())
        {
            return json;
        }
        auto& defaultValue = options.defaultValue;
        if (!defaultValue)
        {
            return json;
        }
        return *defaultValue;
    }

    bool isRequired() const
    {
        auto& required = options.required;
        return required.value_or(false);
    }

    bool isReadOnly() const
    {
        auto& readOnly = options.readOnly;
        return readOnly.value_or(false);
    }

    bool isWriteOnly() const
    {
        auto& writeOnly = options.writeOnly;
        return writeOnly.value_or(false);
    }
};

/**
 * @brief Message metadata.
 *
 */
class MessageInfo
{
public:
    MessageInfo() = default;

    MessageInfo(std::string title)
        : _title(std::move(title))
    {
    }

    JsonSchema getSchema(const void* message) const
    {
        JsonSchema schema;
        schema.title = _title;
        schema.type = JsonType::Object;
        for (const auto& property : _properties)
        {
            property.add(schema, message);
        }
        return schema;
    }

    bool serialize(const void* message, JsonValue& json) const
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& property : _properties)
        {
            if (property.isWriteOnly())
            {
                continue;
            }
            JsonValue child;
            if (property.serialize(message, child))
            {
                object->set(property.name, child);
            }
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
            if (property.isReadOnly())
            {
                continue;
            }
            auto child = property.extract(*object);
            property.deserialize(child, message);
        }
        return true;
    }

    void addProperty(MessageProperty property)
    {
        _properties.push_back(std::move(property));
    }

private:
    std::string _title;
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
#define BRAYNS_NAMED_MESSAGE_BEGIN(TYPE, NAME)                               \
    struct TYPE                                                              \
    {                                                                        \
    private:                                                                 \
        using MessageType = TYPE;                                            \
                                                                             \
        static MessageInfo& _getInfo()                                       \
        {                                                                    \
            static MessageInfo info(NAME);                                   \
            return info;                                                     \
        }                                                                    \
                                                                             \
        static const MessageInfo& _loadInfo()                                \
        {                                                                    \
            static const int buildMessageInfo = []                           \
            {                                                                \
                TYPE();                                                      \
                return 0;                                                    \
            }();                                                             \
            return _getInfo();                                               \
        }                                                                    \
                                                                             \
    public:                                                                  \
        JsonSchema getSchema() const { return _loadInfo().getSchema(this); } \
                                                                             \
        bool serialize(JsonValue& json) const                                \
        {                                                                    \
            return _loadInfo().serialize(this, json);                        \
        }                                                                    \
                                                                             \
        bool deserialize(const JsonValue& json)                              \
        {                                                                    \
            return _loadInfo().deserialize(json, this);                      \
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
#define BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, ...)                         \
    TYPE NAME = []                                                       \
    {                                                                    \
        static const int registerEntry = []                              \
        {                                                                \
            MessageProperty property;                                    \
            property.name = #NAME;                                       \
            property.options = {__VA_ARGS__};                            \
            property.getSchema = [](const void* data)                    \
            {                                                            \
                auto& message = *static_cast<const MessageType*>(data);  \
                return Json::getSchema(message.NAME);                    \
            };                                                           \
            property.serialize = [](const void* data, JsonValue& json)   \
            {                                                            \
                auto& message = *static_cast<const MessageType*>(data);  \
                return Json::serialize(message.NAME, json);              \
            };                                                           \
            property.deserialize = [](const JsonValue& json, void* data) \
            {                                                            \
                auto& message = *static_cast<MessageType*>(data);        \
                return Json::deserialize(json, message.NAME);            \
            };                                                           \
            _getInfo().addProperty(std::move(property));                 \
            return 0;                                                    \
        }();                                                             \
        return TYPE{};                                                   \
    }();

#define BRAYNS_MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION, ...)                    \
    BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, Description(DESCRIPTION), Required(), \
                            __VA_ARGS__)

#define BRAYNS_MESSAGE_OPTION(TYPE, NAME, DESCRIPTION, ...) \
    BRAYNS_MESSAGE_PROPERTY(boost::optional<TYPE>, NAME,    \
                            Description(DESCRIPTION), __VA_ARGS__)

/**
 * @brief Must be called after BRAYNS_MESSAGE_BEGIN and a set of
 * BRAYNS_MESSAGE_ENTRY to generate valid code.
 *
 */
#define BRAYNS_MESSAGE_END() \
    }                        \
    ;
} // namespace brayns
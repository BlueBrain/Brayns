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

#include "Json.h"
#include "JsonSchema.h"

namespace brayns
{
/**
 * @brief Template used to identify messages inside templates to provide
 * automatically a custom JsonSerializer for them. All messages are a
 * specialization of this template.
 *
 * @tparam TagType A tag type to specialize uniquely this template.
 */
template <typename TagType>
struct MessageHolder
{
};

/**
 * @brief Custom JSON serializer for Message specializations. Use internally
 * getMessageInfo() method to perform the serialization.
 *
 * @tparam TagType The tag type provided to MessageHolder<TagType> to identify
 * the message at compile-time.
 */
template <typename TagType>
struct JsonSerializer<MessageHolder<TagType>>
{
    /**
     * @brief Alias for the serialized message type.
     *
     */
    using Message = MessageHolder<TagType>;

    /**
     * @brief Serialize message as an object using Message::serialize.
     *
     * @param value The message to serialize (declared with macros
     * BRAYNS_MESSAGE_BEGIN, BRAYNS_MESSAGE_ENTRY and BRAYNS_MESSAGE_END).
     * @param json The output JSON value.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const Message& value, JsonValue& json)
    {
        return Message::serialize(value, json);
    }

    /**
     * @brief Deserialize message as an object using Message::deserialize.
     *
     * @param json The source JSON value.
     * @param value The message to deserialize (declared with macros
     * BRAYNS_MESSAGE_BEGIN, BRAYNS_MESSAGE_ENTRY and BRAYNS_MESSAGE_END).
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, Message& value)
    {
        return Message::deserialize(json, value);
    }
};

/**
 * @brief Message JSON schemas are created using static method getSchema().
 *
 * @tparam TagType TagType identifying message type in MessageHolder<TagType>.
 */
template <typename TagType>
struct JsonSchemaFactory<MessageHolder<TagType>>
{
    /**
     * @brief Alias for the serialized message type.
     *
     */
    using Message = MessageHolder<TagType>;

    /**
     * @brief Create a JSON schema using Message::getSchema static method.
     *
     * @return JsonSchema JSON schema of the message.
     */
    static JsonSchema createSchema() { return Message::getSchema(); }
};

/**
 * @brief Message property info.
 *
 */
struct MessageProperty
{
    template <typename T>
    static MessageProperty create(std::string name, std::string description)
    {
        MessageProperty property;
        property.schema = JsonSchemaFactory<T>::createSchema();
        property.schema.name = std::move(name);
        property.schema.description = std::move(description);
        return property;
    }

    JsonSchema schema;
    bool required = true;
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
    MessageInfo(std::string title)
    {
        _schema.title = std::move(title);
        _schema.type = JsonTypeName::ofObject();
    }

    const JsonSchema& getSchema() const { return _schema; }

    bool serialize(const void* message, JsonValue& json) const
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& property : _properties)
        {
            JsonValue jsonProperty;
            property.serialize(message, jsonProperty);
            object->set(property.schema.name, jsonProperty);
        }
        json = object;
        return true;
    }

    bool deserialize(const JsonValue& json, void* message) const
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            return false;
        }
        for (const auto& property : _properties)
        {
            auto jsonProperty = object->get(property.schema.name);
            property.deserialize(jsonProperty, message);
        }
        return true;
    }

    void addProperty(const MessageProperty& property)
    {
        _schema.properties.push_back(property.schema);
        if (property.required)
        {
            _schema.required.push_back(property.schema.name);
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
 * BRAYNS_MESSAGE_ENTRY(int, anEntry)
 * BRAYNS_MESSAGE_ENTRY(std::vector<std::string>, someEntries)
 * BRAYNS_MESSAGE_END()
 *
 * // Usage
 * std::string json = Json::stringify(MyMessage());
 * MyMessage message = Json::parse<MyMessage>(json);
 * @endcode
 *
 */
#define BRAYNS_MESSAGE_BEGIN(TYPE)                                      \
    struct TYPE##Tag                                                    \
    {                                                                   \
    };                                                                  \
                                                                        \
    using TYPE = MessageHolder<TYPE##Tag>;                              \
                                                                        \
    template <>                                                         \
    struct MessageHolder<TYPE##Tag>                                     \
    {                                                                   \
    private:                                                            \
        using MessageType = TYPE;                                       \
                                                                        \
        static MessageInfo& _getMessageInfo()                           \
        {                                                               \
            static MessageInfo info(#TYPE);                             \
            return info;                                                \
        }                                                               \
                                                                        \
        static const MessageInfo& _setupAndGetMessageInfo()             \
        {                                                               \
            static const int buildMessageInfo = []                      \
            {                                                           \
                TYPE();                                                 \
                return 0;                                               \
            }();                                                        \
            return _getMessageInfo();                                   \
        }                                                               \
                                                                        \
    public:                                                             \
        static const JsonSchema& getSchema()                            \
        {                                                               \
            return _setupAndGetMessageInfo().getSchema();               \
        }                                                               \
                                                                        \
        static bool serialize(const TYPE& value, JsonValue& json)       \
        {                                                               \
            return _setupAndGetMessageInfo().serialize(&value, json);   \
        }                                                               \
                                                                        \
        static bool deserialize(const JsonValue& json, TYPE& value)     \
        {                                                               \
            return _setupAndGetMessageInfo().deserialize(json, &value); \
        }

/**
 * @brief Add an entry to the current message.
 *
 * Must be called only after BRAYNS_BEGIN_MESSAGE(...). The active message will
 * have a public attribute called NAME of type TYPE, the given description and
 * will be serialized using JsonSerializer<TYPE>.
 *
 */
#define BRAYNS_MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION)                     \
    TYPE NAME = []                                                        \
    {                                                                     \
        static const int registerEntry = []                               \
        {                                                                 \
            MessageProperty property;                                     \
            property.schema = JsonSchemaFactory<TYPE>::createSchema();    \
            property.schema.name = #NAME;                                 \
            property.schema.description = DESCRIPTION;                    \
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
            _getMessageInfo().addProperty(property);                      \
            return 0;                                                     \
        }();                                                              \
        return TYPE{};                                                    \
    }();

/**
 * @brief Must be called after BRAYNS_MESSAGE_BEGIN and a set of
 * BRAYNS_MESSAGE_ENTRY to generate valid code.
 *
 */
#define BRAYNS_MESSAGE_END() \
    }                        \
    ;
} // namespace brayns
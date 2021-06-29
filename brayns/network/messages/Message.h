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

#include <cassert>
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
     * @brief Create a JsonObject::Ptr.
     *
     * Fill it using name and serializer of entries inside getMessageInfo() and
     * put it inside the provided value.
     *
     * @param value The message to serialize (declared with macros
     * BRAYNS_MESSAGE_BEGIN, BRAYNS_MESSAGE_ENTRY and BRAYNS_MESSAGE_END).
     * @param json The output JSON value.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const Message& value, JsonValue& json)
    {
        Message::serialize(value, json);
        return true;
    }

    /**
     * @brief Extract a JsonObject::Ptr from json.
     *
     * Fetch all entries provided by getMessageInfo() using name and serializer
     * and put it inside value. If json is not an object, value is left
     * unchanged, non-matching keys are ignored.
     *
     * @param json The source JSON value.
     * @param value The message to deserialize (declared with macros
     * BRAYNS_MESSAGE_BEGIN, BRAYNS_MESSAGE_ENTRY and BRAYNS_MESSAGE_END).
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, Message& value)
    {
        Message::deserialize(json, value);
        return true;
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
 * @brief Message metadata.
 *
 */
class MessageInfo
{
public:
    using Serializer = std::function<void(const void*, JsonObject&)>;
    using Deserializer = std::function<void(const JsonObject&, void*)>;

    MessageInfo(const std::string& title)
    {
        _schema.title = title;
        _schema.type = JsonTypeName::ofObject();
    }

    const JsonSchema& getSchema() const { return _schema; }

    void serialize(const void* value, JsonValue& json) const
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& serializer : _serializers)
        {
            serializer(value, *object);
        }
        json = object;
    }

    void deserialize(const JsonValue& json, void* value) const
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            return;
        }
        for (const auto& deserializer : _deserializers)
        {
            deserializer(*object, value);
        }
    }

    void addSerializer(const Serializer& serializer)
    {
        _serializers.push_back(serializer);
    }

    void addDeserializer(const Deserializer& deserializer)
    {
        _deserializers.push_back(deserializer);
    }

    template <typename T>
    void addProperty(const std::string& name, const std::string& description)
    {
        _schema.required.push_back(name);
        auto schema = JsonSchemaFactory<T>::createSchema();
        schema.name = name;
        schema.description = description;
        _schema.properties.push_back(std::move(schema));
    }

private:
    JsonSchema _schema;
    std::vector<Serializer> _serializers;
    std::vector<Deserializer> _deserializers;
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
#define BRAYNS_MESSAGE_BEGIN(TYPE)                                  \
    struct TYPE##Tag                                                \
    {                                                               \
    };                                                              \
    using TYPE = MessageHolder<TYPE##Tag>;                          \
    template <>                                                     \
    struct MessageHolder<TYPE##Tag>                                 \
    {                                                               \
    private:                                                        \
        static MessageInfo& _getMessageInfo()                       \
        {                                                           \
            static MessageInfo info(#TYPE);                         \
            return info;                                            \
        }                                                           \
                                                                    \
        static const MessageInfo& _setupAndGetMessageInfo()         \
        {                                                           \
            static const int buildMessageInfo = []                  \
            {                                                       \
                TYPE();                                             \
                return 0;                                           \
            }();                                                    \
            return _getMessageInfo();                               \
        }                                                           \
                                                                    \
    public:                                                         \
        static const JsonSchema& getSchema()                        \
        {                                                           \
            return _setupAndGetMessageInfo().getSchema();           \
        }                                                           \
                                                                    \
        static void serialize(const TYPE& value, JsonValue& json)   \
        {                                                           \
            _setupAndGetMessageInfo().serialize(&value, json);      \
        }                                                           \
                                                                    \
        static void deserialize(const JsonValue& json, TYPE& value) \
        {                                                           \
            _setupAndGetMessageInfo().deserialize(json, &value);    \
        }

/**
 * @brief Add an entry to the current message.
 *
 * Must be called only after BRAYNS_BEGIN_MESSAGE(...). The active message will
 * have a public attribute called NAME of type TYPE, the given description and
 * will be serialized using JsonSerializer<TYPE>.
 *
 */
#define BRAYNS_MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION)                        \
    TYPE NAME = []                                                           \
    {                                                                        \
        static const int registerEntry = []                                  \
        {                                                                    \
            using MessageType = std::decay_t<decltype(*this)>;               \
            auto& info = _getMessageInfo();                                  \
            info.addProperty<TYPE>(#NAME, DESCRIPTION);                      \
            info.addSerializer(                                              \
                [](const void* value, JsonObject& object)                    \
                {                                                            \
                    auto& message = *static_cast<const MessageType*>(value); \
                    object.set(#NAME, Json::serialize(message.NAME));        \
                });                                                          \
            info.addDeserializer(                                            \
                [](const JsonObject& object, void* value)                    \
                {                                                            \
                    auto& message = *static_cast<MessageType*>(value);       \
                    Json::deserialize(object.get(#NAME), message.NAME);      \
                });                                                          \
            return 0;                                                        \
        }();                                                                 \
        return TYPE{};                                                       \
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
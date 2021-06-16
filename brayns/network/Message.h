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

namespace brayns
{
/**
 * @brief Perform the serialization of a message entry. Used to remember an
 * entry type and its position in a message to serialize / deserialize it later.
 *
 */
class EntrySerializer
{
public:
    /**
     * @brief Construct an invalid serializer.
     *
     */
    EntrySerializer() = default;

    /**
     * @brief Construct a serializer from a message and an entry.
     *
     * @tparam MessageType The message type that holds the entry.
     * @tparam EntryType The type of the entry that will be serialized.
     * @param message An instance of the message type.
     * @param entry An instance of the the entry type (used to compute offset
     * from the parent message).
     */
    template <typename MessageType, typename EntryType>
    EntrySerializer(const MessageType& message, const EntryType& entry)
    {
        _messageType = typeid(MessageType);
        _offset = reinterpret_cast<const char*>(&entry) -
                  reinterpret_cast<const char*>(&message);
        _serialize = [](const void* value, JsonValue& json)
        {
            auto& entry = *static_cast<const EntryType*>(value);
            JsonSerializer<EntryType>::serialize(entry, json);
        };
        _deserialize = [](const JsonValue& json, void* value)
        {
            auto& entry = *static_cast<EntryType*>(value);
            JsonSerializer<EntryType>::deserialize(json, entry);
        };
    }

    /**
     * @brief Serialize the entry of the provided message registered during
     * construction to the provided JsonValue.
     *
     * @tparam T The type of the message holding the entry (must be the same a
     * the one at construction).
     * @param message The message holding the entry to serialize.
     * @param json The output json value.
     */
    template <typename T>
    void serialize(const T& message, JsonValue& json) const
    {
        assert(_messageType == typeid(T));
        auto entry = reinterpret_cast<const char*>(&message) + _offset;
        _serialize(entry, json);
    }

    /**
     * @brief Deserialize a JsonValue to the registered entry in the provided
     * message.
     *
     * @tparam T The type of the message (must be the same a the one at
     * construction).
     * @param json The input json value.
     * @param message The message holding the entry to update.
     */
    template <typename T>
    void deserialize(const JsonValue& json, T& message) const
    {
        assert(_messageType == typeid(T));
        auto entry = reinterpret_cast<char*>(&message) + _offset;
        _deserialize(json, entry);
    }

private:
    std::type_index _messageType = typeid(void);
    size_t _offset = 0;
    std::function<void(const void*, JsonValue&)> _serialize;
    std::function<void(const JsonValue&, void*)> _deserialize;
};

/**
 * @brief Describe a message entry and how to serialize it to JSON.
 *
 */
struct EntryInfo
{
    /**
     * @brief The name (JSON key) of the entry.
     *
     */
    std::string name;

    /**
     * @brief A description of the entry.
     *
     */
    std::string description;

    /**
     * @brief A serializer that holds the entry serialization info.
     *
     */
    EntrySerializer serializer;
};

/**
 * @brief Describe a message as a set of EntryInfo.
 *
 */
struct MessageInfo
{
    /**
     * @brief The list of all message entries.
     *
     */
    std::vector<EntryInfo> entries;
};

/**
 * @brief Template used to identify messages inside templates to provide
 * automatically a custom JsonSerializer for them. All messages are a
 * specialization of this template.
 *
 * @tparam TagType A tag type to specialize uniquely this template.
 */
template <typename TagType>
struct Message
{
};

/**
 * @brief Custom JSON serializer for Message specializations. Use internally
 * getMessageInfo() method to perform the serialization.
 *
 * @tparam TagType The tag type provided to Message<TagType> to identify the
 * message at compile-time.
 */
template <typename TagType>
struct JsonSerializer<Message<TagType>>
{
    /**
     * @brief Alias for the serialized message type.
     *
     */
    using Message = Message<TagType>;

    /**
     * @brief Create a JsonObject::Ptr.
     *
     * Fill it using name and serializer of entries inside getMessageInfo() and
     * put it inside the provided value.
     *
     * @param value The message to serialize (declared with macros
     * MESSAGE_BEGIN, MESSAGE_ENTRY and MESSAGE_END).
     * @param json The output JSON value.
     */
    static void serialize(const Message& value, JsonValue& json)
    {
        auto& message = Message::getMessageInfo();
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& entry : message.entries)
        {
            JsonValue child;
            entry.serializer.serialize(value, child);
            object->set(entry.name, child);
        }
        json = object;
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
     * MESSAGE_BEGIN, MESSAGE_ENTRY and MESSAGE_END).
     */
    static void deserialize(const JsonValue& json, Message& value)
    {
        if (json.type() != typeid(JsonObject::Ptr))
        {
            return;
        }
        auto& message = Message::getMessageInfo();
        auto& object = *json.extract<JsonObject::Ptr>();
        for (const auto& entry : message.entries)
        {
            if (!object.has(entry.name))
            {
                continue;
            }
            auto child = object.get(entry.name);
            entry.serializer.deserialize(child, value);
        }
    }
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
 * MESSAGE_BEGIN(MyMessage)
 * MESSAGE_ENTRY(int, anEntry)
 * MESSAGE_ENTRY(std::vector<std::string>, someEntries)
 * MESSAGE_END()
 *
 * // Usage
 * std::string json = Json::stringify(MyMessage());
 * MyMessage message = Json::parse<MyMessage>(json);
 * @endcode
 *
 */
#define MESSAGE_BEGIN(TYPE)                        \
    struct TYPE##Tag                               \
    {                                              \
    };                                             \
    using TYPE = Message<TYPE##Tag>;               \
    template <>                                    \
    struct Message<TYPE##Tag>                      \
    {                                              \
    private:                                       \
        static MessageInfo& _getMessageInfo()      \
        {                                          \
            static MessageInfo info;               \
            return info;                           \
        }                                          \
                                                   \
    public:                                        \
        static const MessageInfo& getMessageInfo() \
        {                                          \
            static const TYPE buildMessageInfo;    \
            return _getMessageInfo();              \
        }

/**
 * @brief Add an entry to the message being built.
 *
 * Must be called only after BEGIN_MESSAGE(...). The active message will have a
 * public attribute called NAME of type TYPE, the given description and will be
 * serialized using JsonSerializer<TYPE>.
 *
 */
#define MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION)         \
    TYPE NAME = [&]                                    \
    {                                                  \
        static const int addEntry = [&]                \
        {                                              \
            EntryInfo entry;                           \
            entry.name = #NAME;                        \
            entry.description = DESCRIPTION;           \
            entry.serializer = {*this, NAME};          \
            auto& entries = _getMessageInfo().entries; \
            entries.push_back(std::move(entry));       \
            return 0;                                  \
        }();                                           \
        return TYPE{};                                 \
    }();

/**
 * @brief Must be called after MESSAGE_BEGIN and a set of MESSAGE_ENTRY to
 * generate valid code.
 *
 */
#define MESSAGE_END() \
    }                 \
    ;
} // namespace brayns
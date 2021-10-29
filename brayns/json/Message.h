/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <brayns/json/adapters/ArrayAdapter.h>
#include <brayns/json/adapters/EnumAdapter.h>
#include <brayns/json/adapters/GlmAdapter.h>
#include <brayns/json/adapters/JsonSchemaAdapter.h>
#include <brayns/json/adapters/MapAdapter.h>
#include <brayns/json/adapters/PrimitiveAdapter.h>
#include <brayns/json/adapters/PtrAdapter.h>

#include <brayns/json/JsonOptions.h>
#include <brayns/json/JsonType.h>

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
        auto object = JsonExtractor::extractObject(json);
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
} // namespace brayns

/**
 * @brief Macro to declare a new message.
 *
 * The resulting message will have the symbol declared in MESSAGE_BEGIN and can
 * be used in JSON serialization with no additional code. A static instance of
 * MessageInfo will be stored inside the resulting message type.
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
#define BRAYNS_NAMED_MESSAGE_BEGIN(TYPE, NAME)           \
    struct TYPE                                          \
    {                                                    \
    private:                                             \
        using MessageType = TYPE;                        \
                                                         \
        static brayns::MessageInfo& _getInfo()           \
        {                                                \
            static brayns::MessageInfo _info(NAME);      \
            return _info;                                \
        }                                                \
                                                         \
        static const brayns::MessageInfo& _loadInfo()    \
        {                                                \
            static std::once_flag _flag;                 \
            std::call_once(_flag, [] { TYPE(); });       \
            return _getInfo();                           \
        }                                                \
                                                         \
    public:                                              \
        brayns::JsonSchema getSchema() const             \
        {                                                \
            return _loadInfo().getSchema(this);          \
        }                                                \
                                                         \
        bool serialize(brayns::JsonValue& _json) const   \
        {                                                \
            return _loadInfo().serialize(this, _json);   \
        }                                                \
                                                         \
        bool deserialize(const brayns::JsonValue& _json) \
        {                                                \
            return _loadInfo().deserialize(_json, this); \
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
#define BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, ...)                              \
    TYPE NAME = [] {                                                          \
        using namespace brayns;                                               \
        static std::once_flag _flag;                                          \
        std::call_once(_flag, [] {                                            \
            MessageProperty _property;                                        \
            _property.name = #NAME;                                           \
            _property.options = {__VA_ARGS__};                                \
            _property.getSchema = [](const void* _data) {                     \
                auto& _message = *static_cast<const MessageType*>(_data);     \
                return Json::getSchema(_message.NAME);                        \
            };                                                                \
            _property.serialize = [](const void* _data, JsonValue& _json) {   \
                auto& _message = *static_cast<const MessageType*>(_data);     \
                return Json::serialize(_message.NAME, _json);                 \
            };                                                                \
            _property.deserialize = [](const JsonValue& _json, void* _data) { \
                auto& _message = *static_cast<MessageType*>(_data);           \
                return Json::deserialize(_json, _message.NAME);               \
            };                                                                \
            _getInfo().addProperty(std::move(_property));                     \
        });                                                                   \
        return TYPE{};                                                        \
    }();

/**
 * @brief Message required entry with mandatory description.
 *
 */
#define BRAYNS_MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION, ...)                \
    BRAYNS_MESSAGE_PROPERTY(TYPE, NAME, brayns::Description(DESCRIPTION), \
                            brayns::Required(), __VA_ARGS__)

/**
 * @brief Message optional entry typed as optional<TYPE>.
 *
 */
#define BRAYNS_MESSAGE_OPTION(TYPE, NAME, DESCRIPTION, ...) \
    BRAYNS_MESSAGE_PROPERTY(boost::optional<TYPE>, NAME,    \
                            brayns::Description(DESCRIPTION), __VA_ARGS__)

/**
 * @brief Must be called after BRAYNS_MESSAGE_BEGIN and a set of
 * BRAYNS_MESSAGE_ENTRY to generate valid code.
 *
 */
#define BRAYNS_MESSAGE_END() \
    }                        \
    ;

#pragma once

#include "JsonObjectInfo.h"

/**
 * @brief Macro to declare a type that can be treated as a JSON object.
 *
 * The resulting message will have the symbol declared in
 * BRAYNS_JSON_OBJECT_BEGIN and can be used in JSON serialization with no
 * additional code. A static instance of JsonObjectInfo will be stored inside
 * the resulting message type.
 *
 * Example:
 * @code {.cpp}
 * // Declaration
 * BRAYNS_JSON_OBJECT_BEGIN(MyMessage)
 * BRAYNS_JSON_OBJECT_ENTRY(int, anEntry, "This is an entry")
 * BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, someEntries, "Descript")
 * BRAYNS_JSON_OBJECT_END()
 *
 * // Usage
 * std::string json = Json::stringify(MyMessage());
 * MyMessage message = Json::parse<MyMessage>(json);
 * @endcode
 *
 */
#define BRAYNS_NAMED_JSON_OBJECT_BEGIN(TYPE, NAME)       \
    struct TYPE                                          \
    {                                                    \
    private:                                             \
        using MyType = TYPE;                             \
                                                         \
        static brayns::JsonObjectInfo& _getInfo()        \
        {                                                \
            static brayns::JsonObjectInfo _info(NAME);   \
            return _info;                                \
        }                                                \
                                                         \
        static const brayns::JsonObjectInfo& _loadInfo() \
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

#define BRAYNS_JSON_OBJECT_BEGIN(TYPE) \
    BRAYNS_NAMED_JSON_OBJECT_BEGIN(TYPE, #TYPE)

/**
 * @brief Add an entry to the current message.
 *
 * Must be called only after BRAYNS_BEGIN_MESSAGE(...). The active message will
 * have a public attribute called NAME of type TYPE, the given description and
 * will be serialized using JsonAdapter<TYPE>.
 *
 */
#define BRAYNS_JSON_OBJECT_PROPERTY(TYPE, NAME, ...)                          \
    TYPE NAME = [] {                                                          \
        using namespace brayns;                                               \
        static std::once_flag _flag;                                          \
        std::call_once(_flag, [] {                                            \
            JsonObjectProperty _property;                                     \
            _property.name = #NAME;                                           \
            _property.options = {__VA_ARGS__};                                \
            _property.getSchema = [](const void* _data) {                     \
                auto& _message = *static_cast<const MyType*>(_data);          \
                return Json::getSchema(_message.NAME);                        \
            };                                                                \
            _property.serialize = [](const void* _data, JsonValue& _json) {   \
                auto& _message = *static_cast<const MyType*>(_data);          \
                return Json::serialize(_message.NAME, _json);                 \
            };                                                                \
            _property.deserialize = [](const JsonValue& _json, void* _data) { \
                auto& _message = *static_cast<MyType*>(_data);                \
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
#define BRAYNS_JSON_OBJECT_ENTRY(TYPE, NAME, DESCRIPTION, ...)                \
    BRAYNS_JSON_OBJECT_PROPERTY(TYPE, NAME, brayns::Description(DESCRIPTION), \
                                brayns::Required(), __VA_ARGS__)

/**
 * @brief Message optional entry typed as optional<TYPE>.
 *
 */
#define BRAYNS_JSON_OBJECT_OPTION(TYPE, NAME, DESCRIPTION, ...) \
    BRAYNS_JSON_OBJECT_PROPERTY(boost::optional<TYPE>, NAME,    \
                                brayns::Description(DESCRIPTION), __VA_ARGS__)

/**
 * @brief Must be called after BRAYNS_JSON_OBJECT_BEGIN and a set of
 * BRAYNS_JSON_OBJECT_ENTRY to generate valid code.
 *
 */
#define BRAYNS_JSON_OBJECT_END() \
    }                            \
    ;

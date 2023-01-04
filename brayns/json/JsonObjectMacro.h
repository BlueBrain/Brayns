#pragma once

#include <mutex>

#include "JsonObjectInfo.h"

/**
 * @brief Macro to declare a type that can be treated as a JSON object.
 *
 * The resulting type will have the symbol declared in BRAYNS_JSON_OBJECT_BEGIN
 * and can be used in JSON serialization with no additional code. A static
 * instance of JsonObjectInfo will be stored inside the resulting type.
 *
 * Example:
 * @code {.cpp}
 * // Declaration
 * BRAYNS_JSON_OBJECT_BEGIN(Type)
 * BRAYNS_JSON_OBJECT_ENTRY(int, anEntry, "This is an entry")
 * BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, someEntries, "Describe")
 * BRAYNS_JSON_OBJECT_END()
 *
 * // Usage
 * std::string json = Json::stringify(Type());
 * Type instance = Json::parse<Type>(json);
 * @endcode
 *
 */
#define BRAYNS_NAMED_JSON_OBJECT_BEGIN(TYPE, NAME) \
    struct TYPE \
    { \
    private: \
        using _MyType = TYPE; \
\
        static inline brayns::JsonObjectInfo _info{NAME}; \
\
    public: \
        static brayns::JsonSchema getSchema() \
        { \
            return _info.getSchema(); \
        } \
\
        void serialize(brayns::JsonValue &_json) const \
        { \
            _info.serialize(this, _json); \
        } \
\
        void deserialize(const brayns::JsonValue &_json) \
        { \
            _info.deserialize(_json, this); \
        }

#define BRAYNS_JSON_OBJECT_BEGIN(TYPE) BRAYNS_NAMED_JSON_OBJECT_BEGIN(TYPE, #TYPE)

/**
 * @brief Add an entry to the current message.
 *
 * Must be called only after BRAYNS_JSON_OBJECT_BEGIN(...). The active message
 * will have a public attribute called NAME of type TYPE, the given description
 * and will be serialized using JsonAdapter<TYPE>.
 *
 */
#define BRAYNS_JSON_OBJECT_PROPERTY(TYPE, NAME, ...) \
public: \
    TYPE NAME{}; \
\
private: \
    static inline int _register##NAME = [] \
    { \
        brayns::JsonObjectProperty _property; \
        _property.name = #NAME; \
        _property.options = brayns::JsonOptions{__VA_ARGS__}; \
        _property.getSchema = [] \
        { \
            using T = std::decay_t<decltype(_MyType::NAME)>; \
            return brayns::Json::getSchema<T>(); \
        }; \
        _property.serialize = [](const void *_data, brayns::JsonValue &_json) \
        { \
            auto &_message = *static_cast<const _MyType *>(_data); \
            brayns::Json::serialize(_message.NAME, _json); \
        }; \
        _property.deserialize = [](const brayns::JsonValue &_json, void *_data) \
        { \
            auto &_message = *static_cast<_MyType *>(_data); \
            brayns::Json::deserialize(_json, _message.NAME); \
        }; \
        _info.addProperty(std::move(_property)); \
        return 0; \
    }();

/**
 * @brief Message required entry with mandatory description.
 *
 */
#define BRAYNS_JSON_OBJECT_ENTRY(TYPE, NAME, DESCRIPTION, ...) \
    BRAYNS_JSON_OBJECT_PROPERTY(TYPE, NAME, brayns::Description(DESCRIPTION), brayns::Required(), __VA_ARGS__)

/**
 * @brief Must be called after BRAYNS_JSON_OBJECT_BEGIN and a set of
 * BRAYNS_JSON_OBJECT_ENTRY to generate valid code.
 *
 */
#define BRAYNS_JSON_OBJECT_END() \
    } \
    ;

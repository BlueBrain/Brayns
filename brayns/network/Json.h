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

#include <algorithm>
#include <cassert>
#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>

#include <brayns/common/mathTypes.h>

namespace brayns
{
/**
 * @brief SON value (object, array, string, number, bool, null).
 *
 */
using JsonValue = Poco::Dynamic::Var;

/**
 * @brief JSON array (vector of JsonValue).
 *
 */
using JsonArray = Poco::JSON::Array;

/**
 * @brief JSON object (map of string and JsonValue).
 *
 */
using JsonObject = Poco::JSON::Object;

/**
 * @brief Template used to serialize and deserialize JSON.
 * @note The default implementation calls Poco serialization and works for all
 * basic types and std::string (see Poco::Dynamic::Var). Will not compile for
 * other types and must hence be specialized.
 *
 * @tparam T The type of the object to serialize / deserialize.
 */
template <typename T>
struct JsonSerializer
{
    /**
     * @brief Serialize an object to a provided JsonValue.
     *
     * @param value The object to serialize (basic type or std::string).
     * @param json The output json value that will contain the object.
     */
    static void serialize(const T& value, JsonValue& json) { json = value; }

    /**
     * @brief Deserialize a JsonValue to a provided object.
     *
     * @param json The source JSON.
     * @param value The output object (basic type or std::string).
     */
    static void deserialize(const JsonValue& json, T& value)
    {
        value = json.convert<T>();
    }
};

/**
 * @brief High level class to serialize / deserialize JSON for custom types. A
 * valid specialization of JsonSerializer<T> is required.
 * 
 */
struct Json
{
    /**
     * @brief Convert a JsonValue to string.
     *
     * @param json The JsonValue to stringify.
     * @return std::string A JSON string representing the given value.
     * @throw Poco::JSON::Exception The JsonValue doesn't contain JSON values.
     */
    static std::string stringify(const JsonValue& json)
    {
        std::ostringstream stream;
        Poco::JSON::Stringifier::condense(json, stream);
        return stream.str();
    }

    /**
     * @brief Parse a JSON string to a JSON value.
     *
     * @param json A JSON string to parse.
     * @return JsonValue The resulting JsonValue
     * @throw Poco::JSON::Exception The JSON format is incorrect.
     */
    static JsonValue parse(const std::string& json)
    {
        Poco::JSON::Parser parser;
        return parser.parse(json);
    }

    /**
     * @brief Serialize a given type to a JsonValue using the
     * JsonSerializer<T>::serialize (must have a valid specialization).
     *
     * @tparam T The type of the object to serialize.
     * @param data The object to serialize.
     * @return JsonValue The JSON representation of the object.
     */
    template <typename T>
    static JsonValue serialize(const T& data)
    {
        JsonValue json;
        JsonSerializer<T>::serialize(data, json);
        return json;
    }

    /**
     * @brief Convert a custom object to JSON string using serialize and
     * stringify.
     *
     * @tparam T The type of the object to serialize.
     * @param data The object to serialize.
     * @return std::string JSON string representing the object.
     */
    template <typename T>
    static std::string stringify(const T& data)
    {
        return stringify(serialize(data));
    }

    /**
     * @brief Convert a JsonValue to a given type using
     * JsonSerializer<T>::serialize (must have a valid specialization).
     *
     * @tparam T The type of the resulting object.
     * @param json The JsonValue containing the object data.
     * @return T The deserialized object.
     */
    template <typename T>
    static T deserialize(const JsonValue& json)
    {
        T data;
        JsonSerializer<T>::deserialize(json, data);
        return data;
    }

    /**
     * @brief Parse a JSON string to a given type using deserialize and parse.
     *
     * @tparam T The resulting object type.
     * @param json The JSON string to parse representing the object.
     * @return T The parsed object.
     */
    template <typename T>
    static T parse(const std::string& json)
    {
        return deserialize<T>(parse(json));
    }
};

/**
 * @brief Specialization of JsonSerializer for vector<T>.
 *
 * @tparam T The type of the vector items.
 */
template <typename T>
struct JsonSerializer<std::vector<T>>
{
    /**
     * @brief Use JsonSerializer<T>::serialize to serialize all items to a JSON
     * array and put it inside the provided JsonValue.
     *
     * @param value The value to serialize.
     * @param json The output JsonValue.
     */
    static void serialize(const std::vector<T>& value, JsonValue& json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (const auto& item : value)
        {
            array->add(Json::serialize(item));
        }
        json = array;
    }

    /**
     * @brief Use JsonSerializer<T>::deserialize to serialize all elements of
     * the JsonArray::Ptr inside the JsonValue to the provided vector. If the
     * json is not an array, the provided value is not changed.
     *
     * @param json The JsonValue to deserialize.
     * @param value The output vector.
     */
    static void deserialize(const JsonValue& json, std::vector<T>& value)
    {
        if (json.type() != typeid(JsonArray::Ptr))
        {
            return;
        }
        auto& array = *json.extract<JsonArray::Ptr>();
        value.resize(array.size());
        for (size_t i = 0; i < value.size(); ++i)
        {
            JsonSerializer<T>::deserialize(array.get(i), value[i]);
        }
    }
};

/**
 * @brief Partial specialization of JsonSerializer for glm::vec<S, T>.
 *
 * @tparam S The size of the vector.
 * @tparam T The type of the vector items.
 */
template <glm::length_t S, typename T>
struct JsonSerializer<vec<S, T>>
{
    /**
     * @brief Create a JsonArray::Ptr, serialize all elements of the vector
     * using JsonSerializer<T>::serialize and put the array inside the provided
     * JsonValue.
     *
     * @param value The vector to serialize.
     * @param json The output JsonValue.
     */
    static void serialize(const glm::vec<S, T>& value, JsonValue& json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (glm::length_t i = 0; i < S; ++i)
        {
            array->add(Json::serialize(value[i]));
        }
        json = array;
    }

    /**
     * @brief Extract a JsonArray::Ptr from the provided JsonValue and
     * deserialize all elements in the provided vector using
     * JsonSerializer<T>::deserialize. If the json is not a JsonArray::Ptr, the
     * value is left unchanged, if its size is not S, only the common indices
     * will be updated (range = min(S, array.size())).
     *
     * @param json The JsonValue to deserialize.
     * @param value The output vector.
     */
    static void deserialize(const JsonValue& json, glm::vec<S, T>& value)
    {
        if (json.type() != typeid(JsonArray::Ptr))
        {
            return;
        }
        auto& array = *json.extract<JsonArray::Ptr>();
        auto size = std::min(S, glm::length_t(array.size()));
        for (glm::length_t i = 0; i < size; ++i)
        {
            JsonSerializer<T>::deserialize(array.get(i), value[i]);
        }
    }
};
} // namespace brayns
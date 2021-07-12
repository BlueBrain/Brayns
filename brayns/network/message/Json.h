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
#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeindex>
#include <deque>
#include <vector>

#include <boost/optional.hpp>

#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>

#include <brayns/common/mathTypes.h>

namespace brayns
{
/**
 * @brief JSON value (object, array, string, number, bool, null).
 *
 */
using JsonValue = Poco::Dynamic::Var;

/**
 * @brief JSON array (vector of JsonValue).
 *
 */
using JsonArray = Poco::JSON::Array;

/**
 * @brief JSON object (map of string to JsonValue).
 *
 */
using JsonObject = Poco::JSON::Object;

/**
 * @brief Helper class to get JSON info about a type.
 *
 * @tparam T Type to analyze.
 */
template <typename T>
struct JsonType
{
    /**
     * @brief Check if T is a primitive JSON type.
     *
     * @return true T is bool, number or string.
     * @return false T is a complex type (array or object).
     */
    static constexpr bool isPrimitive()
    {
        return std::is_arithmetic<T>() || std::is_same<T, std::string>();
    }
};

/**
 * @brief Helper class to extract JSON object / array from JsonValue.
 *
 */
class JsonExtractor
{
public:
    /**
     * @brief Extract a JSON array from a JsonValue if this one is an array.
     *
     * @param json JSON value.
     * @return JsonArray::Ptr JSON array or null if JSON is not an array.
     */
    static JsonArray::Ptr extractArray(const JsonValue& json)
    {
        if (json.type() != typeid(JsonArray::Ptr))
        {
            return nullptr;
        }
        return json.extract<JsonArray::Ptr>();
    }

    /**
     * @brief Extract a JSON object from a JsonValue if this one is an object.
     *
     * @param json JSON value.
     * @return JsoObject::Ptr JSON object or null if JSON is not an object.
     */
    static JsonObject::Ptr extractObject(const JsonValue& json)
    {
        if (json.type() != typeid(JsonObject::Ptr))
        {
            return nullptr;
        }
        return json.extract<JsonObject::Ptr>();
    }
};

/**
 * @brief Template used to serialize and deserialize JSON.
 *
 * The default implementation calls Poco serialization and works for all
 * basic types and std::string (see Poco::Dynamic::Var). Will not compile for
 * other types and must hence be specialized.
 *
 * @tparam T The type of the object to serialize / deserialize.
 */
template <typename T>
struct JsonSerializer
{
    static_assert(JsonType<T>::isPrimitive(),
                  "JSON serialization is not supported for this type, please "
                  "provide a specialization of JsonSerializer<T>");

    /**
     * @brief Serialize an object to a provided JsonValue.
     *
     * @param value The object to serialize (basic type or std::string).
     * @param json The output json value that will contain the object.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        json = value;
        return true;
    }

    /**
     * @brief Deserialize a JsonValue to a provided object.
     *
     * @param json The source JSON.
     * @param value The output object (basic type or std::string).
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        if (!json.isNumeric() && !json.isString())
        {
            return false;
        }
        value = json.convert<T>();
        return true;
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
     * @tparam T Type of the object to serialize.
     * @param value Object to serialize.
     * @return JsonValue JSON source.
     */
    template <typename T>
    static JsonValue serialize(const T& value)
    {
        JsonValue json;
        JsonSerializer<T>::serialize(value, json);
        return json;
    }

    /**
     * @brief Serialize with default value.
     *
     * @tparam T Type of the input object.
     * @param json JsonValue to serialize.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    template <typename T>
    static bool serialize(const T& value, JsonValue& json)
    {
        return JsonSerializer<T>::serialize(value, json);
    }

    /**
     * @brief Convert a custom object to JSON string using serialize and
     * stringify.
     *
     * @tparam T The type of the object to serialize.
     * @param value The object to serialize.
     * @return std::string JSON string representing the object.
     */
    template <typename T>
    static std::string stringify(const T& value)
    {
        return stringify(serialize(value));
    }

    /**
     * @brief Convert a JsonValue to a given type using
     * JsonSerializer<T>::serialize (must have a valid specialization).
     *
     * @tparam T Type of the resulting object.
     * @param json JsonValue containing the object value.
     * @return T Deserialized object.
     */
    template <typename T>
    static T deserialize(const JsonValue& json)
    {
        T value = {};
        JsonSerializer<T>::deserialize(json, value);
        return value;
    }

    /**
     * @brief Deserialize with default value.
     *
     * @tparam T Type of the resulting object.
     * @param json JsonValue containing the object value.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    template <typename T>
    static bool deserialize(const JsonValue& json, T& value)
    {
        JsonSerializer<T>::deserialize(json, value);
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
 * @brief Specialization to do nothing if value is already JSON.
 *
 */
template <>
struct JsonSerializer<JsonValue>
{
    /**
     * @brief Copy value into json.
     *
     * @param value The input JSON.
     * @param json The output JSON.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const JsonValue& value, JsonValue& json)
    {
        json = value;
        return true;
    }

    /**
     * @brief Copy json into value.
     *
     * @param json The input JSON.
     * @param value The output JSON.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, JsonValue& value)
    {
        value = json;
        return true;
    }
};

/**
 * @brief Helper for JsonSerializer of array containers.
 *
 * @tparam T The type of the array container.
 */
template<typename T>
struct ArraySerializer
{
    /**
     * @brief Use JsonSerializer<T>::serialize to serialize all items to a JSON
     * array and put it inside the provided JsonValue.
     *
     * @param value The value to serialize.
     * @param json The output JsonValue.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (const auto& item : value)
        {
            JsonValue jsonItem;
            if (!Json::serialize(item, jsonItem))
            {
                return false;
            }
            array->add(jsonItem);
        }
        json = array;
        return true;
    }

    /**
     * @brief Use JsonSerializer<T>::deserialize to serialize all elements of
     * the JsonArray::Ptr inside the JsonValue to the provided vector. If the
     * json is not an array, the provided value is not changed.
     *
     * @param json The JsonValue to deserialize.
     * @param value The output vector.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        auto array = JsonExtractor::extractArray(json);
        if (!array)
        {
            return false;
        }
        T buffer(array->size());
        for (size_t i = 0; i < array->size(); ++i)
        {
            if (!Json::deserialize(array->get(i), buffer[i]))
            {
                return false;
            }
        }
        value = std::move(buffer);
        return true;
    }
};

/**
 * @brief Specialization of JsonSerializer for vector<T>.
 *
 * @tparam T The type of the vector items.
 */
template <typename T>
struct JsonSerializer<std::vector<T>> : public ArraySerializer<std::vector<T>>
{
};

/**
 * @brief Specialization of JsonSerializer for deque<T>.
 *
 * @tparam T The type of the deque items.
 */
template <typename T>
struct JsonSerializer<std::deque<T>> : public ArraySerializer<std::deque<T>>
{
};

/**
 * @brief Helper class to serialize GLM types to a JSON array.
 *
 * @tparam T GLM type.
 */
template<typename T>
struct GlmSerializer
{
    /**
     * @brief Create a JsonArray::Ptr.
     *
     * Derialize all elements of the vector using JsonSerializer<T>::serialize
     * and put the array inside the provided JsonValue.
     *
     * @param value The vector to serialize.
     * @param json The output JsonValue.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (glm::length_t i = 0; i < value.length(); ++i)
        {
            JsonValue jsonItem;
            Json::serialize(value[i], jsonItem);
            array->add(jsonItem);
        }
        json = array;
        return true;
    }

    /**
     * @brief Extract a JsonArray::Ptr from the provided JsonValue.
     *
     * Deserialize all elements in the provided vector using
     * JsonSerializer<T>::deserialize. If the json is not a JsonArray::Ptr, the
     * value is left unchanged, if its size is not S, only the common indices
     * will be updated (range = min(S, array.size())).
     *
     * @param json The JsonValue to deserialize.
     * @param value The output vector.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        auto array = JsonExtractor::extractArray(json);
        if (!array)
        {
            return false;
        }
        auto size = std::min(value.length(), glm::length_t(array->size()));
        for (glm::length_t i = 0; i < size; ++i)
        {
            Json::deserialize(array->get(i), value[i]);
        }
        return true;
    }
};

/**
 * @brief Partial specialization of JsonSerializer for glm::vec<S, T>.
 *
 * @tparam S Size of the vector.
 * @tparam T Type of the vector components.
 */
template <glm::length_t S, typename T>
struct JsonSerializer<glm::vec<S, T>> : public GlmSerializer<glm::vec<S, T>>
{
};

/**
 * @brief Partial specialization of JsonSerializer for glm::qua<T>.
 *
 * @tparam T Type of the quaternion components.
 */
template <typename T>
struct JsonSerializer<glm::qua<T>> : public GlmSerializer<glm::qua<T>>
{
};

/**
 * @brief Partial specialization of JsonSerializer for Box<T>.
 *
 * @tparam T Real type (float or double).
 */
template<typename T>
struct JsonSerializer<Box<T>>
{
    /**
     * @brief Serialize value in json.
     * 
     * @param value Input value.
     * @param json Output JSON.
     * @return true Always.
     * @return false Never.
     */
    static bool serialize(const Box<T>& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        object->set("min", Json::serialize(value.getMin()));
        object->set("max", Json::serialize(value.getMax()));
        json = object;
        return true;
    }

    /**
     * @brief Deserialize json in value.
     * 
     * @param json Input JSON.
     * @param value Output box (unchanged if failure).
     * @return true Success.
     * @return false Failure (wrong types in json).
     */
    static bool deserialize(const JsonValue& json, Box<T>& value)
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            return false;
        }
        typename Box<T>::vec min;
        if (!Json::deserialize(object->get("min"), min))
        {
            return false;
        }
        typename Box<T>::vec max;
        if (!Json::deserialize(object->get("max"), max))
        {
            return false;
        }
        value = {min, max};
        return true;
    }
};

/**
 * @brief Partial specialization of JsonSerializer for optional.
 *
 * @tparam T Contained type.
 */
template <typename T>
struct JsonSerializer<boost::optional<T>>
{
    /**
     * @brief Serialize contained value if not null.
     *
     * @param value The value to serialize.
     * @param json The output JsonValue.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const boost::optional<T>& value, JsonValue& json)
    {
        if (!value)
        {
            return false;
        }
        return Json::serialize(*value, json);
    }

    /**
     * @brief Deserialize contained value or set it to null.
     *
     * @param json The JsonValue to deserialize.
     * @param value The output value.
     * @return true if success, false if failure, value is null in this case.
     */
    static bool deserialize(const JsonValue& json, boost::optional<T>& value)
    {
        T buffer = {};
        if (!Json::deserialize(json, buffer))
        {
            value = {};
            return false;
        }
        value = std::move(buffer);
        return true;
    }
};
} // namespace brayns
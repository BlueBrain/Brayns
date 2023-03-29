/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

#include <brayns/json/JsonAdapter.h>

namespace brayns
{
/**
 * @brief Helper class to handle JSON primitives (boolean, number and string).
 *
 * @tparam T Primitive type.
 */
template<typename T>
struct PrimitiveAdapter
{
    static JsonSchema getSchema()
    {
        static constexpr auto type = JsonTypeInfo::getType<T>();
        if constexpr (type == JsonType::Undefined)
        {
            return JsonSchema::from(WildcardSchema());
        }
        else if constexpr (type == JsonType::Null)
        {
            return JsonSchema::from(NullSchema());
        }
        else if constexpr (type == JsonType::Boolean)
        {
            return JsonSchema::from(BooleanSchema());
        }
        else if constexpr (type == JsonType::Integer)
        {
            return JsonSchema::from(IntegerSchema(_getMin<T>(), _getMax<T>()));
        }
        else if constexpr (type == JsonType::Number)
        {
            return JsonSchema::from(NumberSchema(_getMin<T>(), _getMax<T>()));
        }
        else if constexpr (type == JsonType::String)
        {
            return JsonSchema::from(StringSchema());
        }
        else
        {
            static_assert(_alwaysFalse<T>, "Not a primitive type");
        }
    }

    static void serialize(const T &value, JsonValue &json)
    {
        if constexpr (std::is_same_v<T, EmptyJson>)
        {
            json.clear();
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            json = _checkInfAndNan(value);
        }
        else
        {
            json = value;
        }
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        if constexpr (std::is_same_v<T, JsonValue>)
        {
            value = json;
        }
        else if constexpr (!std::is_same_v<T, EmptyJson>)
        {
            value = json.convert<T>();
        }
    }

private:
    template<typename U>
    static constexpr auto _alwaysFalse = false;

    template<typename U>
    static constexpr double _getMin()
    {
        auto result = std::numeric_limits<U>::lowest();
        return static_cast<double>(result);
    }

    template<typename U>
    static constexpr double _getMax()
    {
        auto result = std::numeric_limits<U>::max();
        return static_cast<double>(result);
    }

    template<typename U>
    static U _checkInfAndNan(U value)
    {
        if (std::isinf(value))
        {
            return std::numeric_limits<U>::max();
        }
        if (std::isnan(value))
        {
            return U{0};
        }
        return value;
    }
};

/**
 * @brief JSON handling for undefined types.
 *
 */
template<>
struct JsonAdapter<JsonValue> : PrimitiveAdapter<JsonValue>
{
};

/**
 * @brief JSON handling for empty JSON.
 *
 */
template<>
struct JsonAdapter<EmptyJson> : PrimitiveAdapter<EmptyJson>
{
};

/**
 * @brief JSON handling for bool.
 *
 */
template<>
struct JsonAdapter<bool> : PrimitiveAdapter<bool>
{
};

/**
 * @brief JSON handling for int8.
 *
 */
template<>
struct JsonAdapter<int8_t> : PrimitiveAdapter<int8_t>
{
};

/**
 * @brief JSON handling for uint8.
 *
 */
template<>
struct JsonAdapter<uint8_t> : PrimitiveAdapter<uint8_t>
{
};

/**
 * @brief JSON handling for int16.
 *
 */
template<>
struct JsonAdapter<int16_t> : PrimitiveAdapter<int16_t>
{
};

/**
 * @brief JSON handling for uint16.
 *
 */
template<>
struct JsonAdapter<uint16_t> : PrimitiveAdapter<uint16_t>
{
};

/**
 * @brief JSON handling for int32.
 *
 */
template<>
struct JsonAdapter<int32_t> : PrimitiveAdapter<int32_t>
{
};

/**
 * @brief JSON handling for uint64.
 *
 */
template<>
struct JsonAdapter<uint32_t> : PrimitiveAdapter<uint32_t>
{
};

/**
 * @brief JSON handling for int64.
 *
 */
template<>
struct JsonAdapter<int64_t> : PrimitiveAdapter<int64_t>
{
};

/**
 * @brief JSON handling for uint64.
 *
 */
template<>
struct JsonAdapter<uint64_t> : PrimitiveAdapter<uint64_t>
{
};

/**
 * @brief JSON handling for float.
 *
 */
template<>
struct JsonAdapter<float> : PrimitiveAdapter<float>
{
};

/**
 * @brief JSON handling for double.
 *
 */
template<>
struct JsonAdapter<double> : PrimitiveAdapter<double>
{
};

/**
 * @brief JSON handling for string.
 *
 */
template<>
struct JsonAdapter<std::string> : PrimitiveAdapter<std::string>
{
};
} // namespace brayns

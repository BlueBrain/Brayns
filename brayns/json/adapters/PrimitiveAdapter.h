/* Copyright 2015-2024 Blue Brain Project/EPFL
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
#include <cstddef>
#include <cstdint>

#include <brayns/json/Json.h>
#include <brayns/json/JsonExtractor.h>

namespace brayns
{
/**
 * @brief Helper class to reflect primitives (numbers and strings).
 *
 * @tparam T Primitive type.
 */
template <typename T>
struct PrimitiveAdapter
{
    /**
     * @brief Create a JSON schema with type and minimum if unsigned.
     *
     * @return JsonSchema Schema.
     */
    static JsonSchema getSchema(const T&)
    {
        JsonSchema schema;
        schema.type = GetJsonType::fromPrimitive<T>();
        if (!std::is_same<T, bool>() && std::is_unsigned<T>())
        {
            schema.minimum = 0.0;
        }
        return schema;
    }

    /**
     * @brief Serialize contained value using JSON backend.
     *
     * @param value Value to serialize.
     * @param json Output JsonValue.
     * @return true Success,
     * @return false Failure.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        json = value;
        return true;
    }

    /**
     * @brief Deserialize contained value using JSON backend.
     *
     * @param json JsonValue to deserialize.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
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
 * @brief JSON adapter for floating point numbers.
 *
 * @tparam T Type (usually float or double).
 */
template <typename T>
struct FloatAdapter : PrimitiveAdapter<T>
{
    /**
     * @brief Override serialization to handle inf and nan.
     *
     * @param value Input value.
     * @param json Ouput JSON.
     * @return true Success.
     * @return false Failure.
     */
    static bool serialize(T value, JsonValue& json)
    {
        if (std::isinf(value))
        {
            json = std::numeric_limits<T>::max();
            return true;
        }
        if (std::isnan(value))
        {
            json = T{};
            return true;
        }
        json = value;
        return true;
    }
};

/**
 * @brief Allow boolean JSON handling.
 *
 */
template <>
struct JsonAdapter<bool> : PrimitiveAdapter<bool>
{
};

/**
 * @brief Allow int8 JSON handling.
 *
 */
template <>
struct JsonAdapter<int8_t> : PrimitiveAdapter<int8_t>
{
};

/**
 * @brief Allow uint8 JSON handling.
 *
 */
template <>
struct JsonAdapter<uint8_t> : PrimitiveAdapter<uint8_t>
{
};

/**
 * @brief Allow int16 JSON handling.
 *
 */
template <>
struct JsonAdapter<int16_t> : PrimitiveAdapter<int16_t>
{
};

/**
 * @brief Allow uint16 JSON handling.
 *
 */
template <>
struct JsonAdapter<uint16_t> : PrimitiveAdapter<uint16_t>
{
};

/**
 * @brief Allow int32 JSON handling.
 *
 */
template <>
struct JsonAdapter<int32_t> : PrimitiveAdapter<int32_t>
{
};

/**
 * @brief Allow uint64 JSON handling.
 *
 */
template <>
struct JsonAdapter<uint32_t> : PrimitiveAdapter<uint32_t>
{
};

/**
 * @brief Allow int64 JSON handling.
 *
 */
template <>
struct JsonAdapter<int64_t> : PrimitiveAdapter<int64_t>
{
};

/**
 * @brief Allow uint64 JSON handling.
 *
 */
template <>
struct JsonAdapter<uint64_t> : PrimitiveAdapter<uint64_t>
{
};

/**
 * @brief Allow float JSON handling
 *
 */
template <>
struct JsonAdapter<float> : FloatAdapter<float>
{
};

/**
 * @brief Allow double JSON handling.
 *
 */
template <>
struct JsonAdapter<double> : FloatAdapter<double>
{
};

/**
 * @brief Allow string JSON handling.
 *
 */
template <>
struct JsonAdapter<std::string> : PrimitiveAdapter<std::string>
{
};
} // namespace brayns

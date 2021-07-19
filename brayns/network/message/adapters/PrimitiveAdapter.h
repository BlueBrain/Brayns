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

#include <cstddef>
#include <cstdint>

#include <brayns/network/message/Json.h>

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
    static_assert(JsonTypeHelper::isPrimitive<T>(), "Not a primitive");

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

template <>
struct JsonAdapter<bool> : PrimitiveAdapter<bool>
{
};

template <>
struct JsonAdapter<int8_t> : PrimitiveAdapter<int8_t>
{
};

template <>
struct JsonAdapter<uint8_t> : PrimitiveAdapter<uint8_t>
{
};

template <>
struct JsonAdapter<int16_t> : PrimitiveAdapter<int16_t>
{
};

template <>
struct JsonAdapter<uint16_t> : PrimitiveAdapter<uint16_t>
{
};

template <>
struct JsonAdapter<int32_t> : PrimitiveAdapter<int32_t>
{
};

template <>
struct JsonAdapter<uint32_t> : PrimitiveAdapter<uint32_t>
{
};

template <>
struct JsonAdapter<int64_t> : PrimitiveAdapter<int64_t>
{
};

template <>
struct JsonAdapter<uint64_t> : PrimitiveAdapter<uint64_t>
{
};

template <>
struct JsonAdapter<float> : PrimitiveAdapter<float>
{
};

template <>
struct JsonAdapter<double> : PrimitiveAdapter<double>
{
};

template <>
struct JsonAdapter<std::string> : PrimitiveAdapter<std::string>
{
};
} // namespace brayns
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

#include <cstdint>
#include <cstddef>

#include <brayns/network/message/Json.h>

namespace brayns
{
/**
 * @brief Helper class to reflect primitives (numbers and strings).
 *
 * @tparam T Primitive type.
 */
template <typename T>
struct PrimitiveReflector
{
    static_assert(JsonTypeInfo::isPrimitive<T>(),
                  "JSON reflection is not supported for this type, please "
                  "provide a valid specialization of JsonReflector<T>");

    /**
     * @brief Create a JSON schema with type and minimum if unsigned.
     *
     * @return JsonSchema Schema.
     */
    static JsonSchema getSchema(const T&)
    {
        JsonSchema schema;
        schema.type = GetJsonTypeName::fromPrimitive<T>();
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
struct JsonReflector<bool> : PrimitiveReflector<bool>
{
};

template <>
struct JsonReflector<int8_t> : PrimitiveReflector<int8_t>
{
};

template <>
struct JsonReflector<uint8_t> : PrimitiveReflector<uint8_t>
{
};

template <>
struct JsonReflector<int16_t> : PrimitiveReflector<int16_t>
{
};

template <>
struct JsonReflector<uint16_t> : PrimitiveReflector<uint16_t>
{
};

template <>
struct JsonReflector<int32_t> : PrimitiveReflector<int32_t>
{
};

template <>
struct JsonReflector<uint32_t> : PrimitiveReflector<uint32_t>
{
};

template <>
struct JsonReflector<int64_t> : PrimitiveReflector<int64_t>
{
};

template <>
struct JsonReflector<uint64_t> : PrimitiveReflector<uint64_t>
{
};

template <>
struct JsonReflector<float> : PrimitiveReflector<float>
{
};

template <>
struct JsonReflector<double> : PrimitiveReflector<double>
{
};

template <>
struct JsonReflector<std::string> : PrimitiveReflector<std::string>
{
};
} // namespace brayns
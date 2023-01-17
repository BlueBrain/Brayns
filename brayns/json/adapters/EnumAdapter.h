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

#include <utility>
#include <vector>

#include <brayns/utils/EnumInfo.h>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Base JSON adapter for enumerations.
 *
 * @tparam T Enum type.
 */
template<typename T>
struct EnumAdapter
{
    /**
     * @brief Create an enum JSON schema.
     *
     * An enum schema is a string with all names in EnumMap<T> as available
     * values.
     *
     * @return JsonSchema Schema of enum type.
     */
    static JsonSchema getSchema()
    {
        JsonSchema schema;
        schema.type = JsonType::String;
        schema.enums = EnumInfo::getNames<T>();
        return schema;
    }

    /**
     * @brief Serialize an enum value using its name.
     *
     * @param value Input value.
     * @param json Output JSON.
     */
    static void serialize(const T &value, JsonValue &json)
    {
        json = EnumInfo::getName(value);
    }

    /**
     * @brief Deserialize an enum value from its name.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, T &value)
    {
        auto name = Json::deserialize<std::string>(json);
        value = EnumInfo::getValue<T>(name);
    }
};

/**
 * @brief Shortcut to reflect an enum with all its available name-value pairs.
 *
 * Reflected enum can be used in JSON (schema, serialization) and GetEnumName.
 *
 * @code {.cpp}
 * BRAYNS_JSON_ADAPTER_ENUM(MyEnum,
 *      {"Value1": MyEnum::Value1},
 *      {"Value2", MyEnum::Value2})
 * @endcode
 *
 */
#define BRAYNS_JSON_ADAPTER_ENUM(TYPE, ...) \
    template<> \
    struct EnumReflector<TYPE> \
    { \
        static EnumMap<TYPE> reflect() \
        { \
            return {__VA_ARGS__}; \
        } \
    }; \
\
    template<> \
    struct JsonAdapter<TYPE> : EnumAdapter<TYPE> \
    { \
    };

/**
 * @brief Reflect JsonType enum.
 *
 */
template<>
struct JsonAdapter<JsonType> : EnumAdapter<JsonType>
{
};
} // namespace brayns

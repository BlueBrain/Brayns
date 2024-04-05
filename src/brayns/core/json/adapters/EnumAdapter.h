/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/utils/EnumInfo.h>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Base JSON adapter for enumerations relying on EnumInfo.
 *
 * @tparam T Enum type.
 */
template<typename T>
struct EnumAdapter
{
    static JsonSchema getSchema()
    {
        auto schema = JsonSchema();
        schema.type = JsonType::String;
        schema.enums = EnumInfo::getNames<T>();
        return schema;
    }

    static void serialize(const T &value, JsonValue &json)
    {
        json = EnumInfo::getName(value);
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        auto name = json.extract<std::string>();
        value = EnumInfo::getValue<T>(name);
    }
};

/**
 * @brief JSON handling for JsonType.
 *
 */
template<>
struct JsonAdapter<JsonType> : EnumAdapter<JsonType>
{
};
} // namespace brayns

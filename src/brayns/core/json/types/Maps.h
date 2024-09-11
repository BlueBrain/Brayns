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

#include <map>
#include <string>
#include <unordered_map>

#include "Primitives.h"

namespace brayns
{
template<typename T>
struct JsonMapReflector
{
    using ValueType = typename T::mapped_type;

    static JsonSchema getSchema()
    {
        return {
            .type = JsonType::Object,
            .items = {getJsonSchema<ValueType>()},
        };
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto object = createJsonObject();

        for (const auto &[key, item] : value)
        {
            auto jsonItem = serializeToJson(item);
            object->set(key, jsonItem);
        }

        json = object;
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        const auto &object = getObject(json);
        value.clear();

        for (const auto &[key, jsonItem] : object)
        {
            value[key] = deserializeJsonAs<ValueType>(jsonItem);
        }
    }
};

template<typename T>
struct JsonReflector<std::map<std::string, T>> : JsonMapReflector<std::map<std::string, T>>
{
};

template<typename T>
struct JsonReflector<std::unordered_map<std::string, T>> : JsonMapReflector<std::unordered_map<std::string, T>>
{
};
}

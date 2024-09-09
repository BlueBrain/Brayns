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

#include <deque>
#include <list>
#include <vector>

#include "Primitives.h"

namespace brayns
{
template<typename T>
struct JsonArrayReflector
{
    using ValueType = typename T::value_type;

    static JsonSchema getSchema()
    {
        return {
            .type = JsonType::Array,
            .items = {getJsonSchema<ValueType>()},
        };
    }

    static JsonValue serialize(const T &value)
    {
        auto array = createJsonArray();

        for (const auto &item : value)
        {
            auto jsonItem = serializeToJson(item);
            array->add(jsonItem);
        }

        return array;
    }

    static T deserialize(const JsonValue &json)
    {
        const auto &array = getArray(json);

        auto value = T();

        for (const auto &jsonItem : array)
        {
            auto item = deserializeAs<ValueType>(jsonItem);
            value.push_back(std::move(item));
        }

        return value;
    }
};

template<typename T>
struct JsonReflector<std::vector<T>> : JsonArrayReflector<std::vector<T>>
{
};

template<typename T>
struct JsonReflector<std::deque<T>> : JsonArrayReflector<std::deque<T>>
{
};

template<typename T>
struct JsonReflector<std::list<T>> : JsonArrayReflector<std::list<T>>
{
};
}

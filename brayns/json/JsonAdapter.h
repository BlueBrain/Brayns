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

#include "JsonSchema.h"
#include "JsonType.h"

namespace brayns
{
/**
 * @brief Default JsonAdapter intrusive implementation.
 *
 * Use methods getSchema(), serialize(JsonValue&) and deserialize(const
 * JsonValue&) of the instance of T to reflect JSON info from it.
 *
 * @tparam T Type to reflect.
 */
template <typename T>
struct JsonAdapter
{
    /**
     * @brief Get the JSON schema using getSchema() method of value.
     *
     * @param value Value to get the schema from.
     * @return JsonSchema JSON schema of value.
     */
    static JsonSchema getSchema(const T& value) { return value.getSchema(); }

    /**
     * @brief Serialize value into json.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @return true Success.
     * @return false Failure.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        return value.serialize(json);
    }

    /**
     * @brief Deserialize json into value.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        return value.deserialize(json);
    }
};
} // namespace brayns

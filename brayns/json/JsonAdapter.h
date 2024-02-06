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

#include "JsonSchema.h"
#include "JsonType.h"

namespace brayns
{
/**
 * @brief Default JsonAdapter intrusive implementation.
 *
 * Use static method T::getSchema() to get the schema.
 * Use method value.serialize(json) to serialize.
 * Use method value.deserialize(json) to deserialize.
 *
 * @tparam T Type to adapt for JSON usage.
 */
template<typename T>
struct JsonAdapter
{
    /**
     * @brief Get the JSON schema using getSchema() static method of T.
     *
     * @return JsonSchema JSON schema of type.
     */
    static JsonSchema getSchema()
    {
        return T::getSchema();
    }

    /**
     * @brief Serialize value into json.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @throw std::exception Invalid value.
     */
    static void serialize(const T &value, JsonValue &json)
    {
        value.serialize(json);
    }

    /**
     * @brief Deserialize json into value.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @throw std::exception Invalid JSON.
     */
    static void deserialize(const JsonValue &json, T &value)
    {
        value.deserialize(json);
    }
};
} // namespace brayns

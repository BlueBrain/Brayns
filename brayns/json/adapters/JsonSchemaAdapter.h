/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapter.h>

namespace brayns
{
/**
 * @brief Adapt JsonSchema to be used as JSON objects.
 *
 */
template<>
struct JsonAdapter<JsonSchema>
{
    /**
     * @brief Return the schema itself.
     *
     * @param schema Input schema.
     * @return JsonSchema Output schema.
     */
    static JsonSchema getSchema(const JsonSchema &schema);

    /**
     * @brief Serialize a JSON schema as a JSON object
     *
     * @param value Input value.
     * @param json Ouput JSON.
     * @return true Success.
     * @return false Failure.
     */
    static bool serialize(const JsonSchema &value, JsonValue &json);

    /**
     * @brief Deserialize a JSON schema from a JSON object.
     *
     * @param json Input JSON.
     * @param value Ouput value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue &json, JsonSchema &value);
};
} // namespace brayns

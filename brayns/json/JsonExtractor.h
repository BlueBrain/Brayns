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

#include "JsonType.h"

namespace brayns
{
/**
 * @brief Helper class to extract JSON elements from a JSON value.
 *
 */
class JsonExtractor
{
public:
    /**
     * @brief Extract an array if json is one.
     *
     * @param json JSON source.
     * @return JsonArray::Ptr JSON array or null if not an array.
     */
    static JsonArray::Ptr extractArray(const JsonValue& json)
    {
        if (json.type() != typeid(JsonArray::Ptr))
        {
            return nullptr;
        }
        return json.extract<JsonArray::Ptr>();
    }

    /**
     * @brief Extract an object if json is one.
     *
     * @param json JSON source.
     * @return JsonObject::Ptr JSON object or null if not an object.
     */
    static JsonObject::Ptr extractObject(const JsonValue& json)
    {
        if (json.type() != typeid(JsonObject::Ptr))
        {
            return nullptr;
        }
        return json.extract<JsonObject::Ptr>();
    }
};
} // namespace brayns
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

#include "JsonSchema.h"
#include "JsonType.h"

namespace brayns
{
/**
 * @brief Default JsonReflector intrusive implementation.
 *
 * Use methods getSchema(), serialize(JsonValue&) and deserialize(const
 * JsonValue&) of the instance of T to reflect JSON info from it.
 *
 * @tparam T Type to reflect.
 */
template <typename T>
struct JsonReflector
{
    static JsonSchema getSchema(const T& value) { return value.getSchema(); }

    static bool serialize(const T& value, JsonValue& json)
    {
        return value.serialize(json);
    }

    static bool deserialize(const JsonValue& json, T& value)
    {
        return value.deserialize(json);
    }
};
} // namespace brayns
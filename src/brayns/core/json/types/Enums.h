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

#include <type_traits>

#include <brayns/core/utils/EnumReflector.h>

#include "Primitives.h"

namespace brayns
{
template<ReflectedEnum T>
struct JsonReflector<T>
{
    static JsonSchema getSchema()
    {
        const auto &fields = getEnumFields<T>();

        auto oneOf = std::vector<JsonSchema>();
        oneOf.reserve(fields.size());

        for (const auto &field : fields)
        {
            oneOf.push_back({
                .description = field.description,
                .type = JsonType::String,
                .constant = field.name,
            });
        }

        return {.oneOf = std::move(oneOf)};
    }

    static JsonValue serialize(const T &value)
    {
        return getEnumName(value);
    }

    static T deserialize(const JsonValue &json)
    {
        auto name = deserializeAs<std::string>(json);

        try
        {
            return getEnumValue<T>(name);
        }
        catch (const std::exception &e)
        {
            throw JsonException(e.what());
        }
    }
};
}

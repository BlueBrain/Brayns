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

#include <optional>
#include <variant>

#include <brayns/core/jsonv2/JsonReflector.h>

namespace brayns::experimental
{
template<typename T>
struct JsonReflector<std::optional<T>>
{
    static JsonSchema getSchema()
    {
        return {
            .required = false,
            .oneOf = {getJsonSchema<T>(), getJsonSchema<NullJson>()},
        };
    }

    static JsonValue serialize(const std::optional<T> &value)
    {
        if (!value)
        {
            return {};
        }
        return serializeToJson<T>(*value);
    }

    static std::optional<T> deserialize(const JsonValue &json)
    {
        if (json.isEmpty())
        {
            return std::nullopt;
        }
        return deserializeAs<T>(json);
    }
};

template<typename... Ts>
struct JsonReflector<std::variant<Ts...>>
{
    static JsonSchema getSchema()
    {
        return {
            .oneOf = {getJsonSchema<Ts>()...},
        };
    }

    static JsonValue serialize(const std::variant<Ts...> &value)
    {
        return std::visit([](const auto &item) { return serializeToJson(item); }, value);
    }

    static std::variant<Ts...> deserialize(const JsonValue &json)
    {
        return tryDeserialize<Ts...>(json);
    }

private:
    template<typename U, typename... Us>
    static std::variant<Ts...> tryDeserialize(const JsonValue &json)
    {
        try
        {
            return deserializeAs<U>(json);
        }
        catch (...)
        {
            if constexpr (sizeof...(Us) == 0)
            {
                throw JsonException("Invalid oneOf");
            }
            else
            {
                return tryDeserialize<Us...>(json);
            }
        }
    }
};
}

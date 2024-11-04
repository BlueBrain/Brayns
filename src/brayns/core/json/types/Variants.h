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

#include <brayns/core/json/JsonReflector.h>

namespace brayns
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

    static void serialize(const std::optional<T> &value, JsonValue &json)
    {
        if (!value)
        {
            json.clear();
            return;
        }

        json = serializeToJson<T>(*value);
    }

    static void deserialize(const JsonValue &json, std::optional<T> &value)
    {
        if (json.isEmpty())
        {
            value.reset();
            return;
        }

        value = deserializeJsonAs<T>(json);
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

    static void serialize(const std::variant<Ts...> &value, JsonValue &json)
    {
        std::visit([&](const auto &item) { serializeToJson(item, json); }, value);
    }

    static void deserialize(const JsonValue &json, std::variant<Ts...> &value)
    {
        tryDeserialize<Ts...>(json, value);
    }

private:
    template<typename U, typename... Us>
    static void tryDeserialize(const JsonValue &json, std::variant<Ts...> &value)
    {
        try
        {
            value = deserializeJsonAs<U>(json);
        }
        catch (...)
        {
            if constexpr (sizeof...(Us) == 0)
            {
                throw JsonException("Invalid oneOf");
            }
            else
            {
                return tryDeserialize<Us...>(json, value);
            }
        }
    }
};
}

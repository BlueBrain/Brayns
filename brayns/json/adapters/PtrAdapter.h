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

#include <memory>
#include <optional>

#include <brayns/json/JsonAdapter.h>

namespace brayns
{
/**
 * @brief Helper for JsonAdapter for pointer-like values.
 *
 * @tparam T Contained type.
 */
template<typename T>
struct PtrAdapter
{
    using ValueType = std::decay_t<decltype(*T{})>;

    static JsonSchema getSchema()
    {
        return JsonAdapter<ValueType>::getSchema();
    }

    static void serialize(const T &value, JsonValue &json)
    {
        if (!value)
        {
            json.clear();
            return;
        }
        JsonAdapter<ValueType>::serialize(*value, json);
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        if (json.isEmpty())
        {
            value = {};
            return;
        }
        if (!value)
        {
            return;
        }
        JsonAdapter<ValueType>::deserialize(json, *value);
    }
};

/**
 * @brief JSON handling for raw pointers.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<T *> : PtrAdapter<T *>
{
};

/**
 * @brief JSON handling for std::unique_ptr<T>.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<std::unique_ptr<T>> : PtrAdapter<std::unique_ptr<T>>
{
    static void deserialize(const JsonValue &json, std::unique_ptr<T> &value)
    {
        if (json.isEmpty())
        {
            value = {};
            return;
        }
        if (!value)
        {
            value = std::make_unique<T>();
        }
        JsonAdapter<T>::deserialize(json, *value);
    }
};

/**
 * @brief JSON handling for std::shared_ptr<T>.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<std::shared_ptr<T>> : PtrAdapter<std::shared_ptr<T>>
{
    /**
     * @brief Create value using make_shared<T>() if empty.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, std::shared_ptr<T> &value)
    {
        if (json.isEmpty())
        {
            value = {};
            return;
        }
        if (!value)
        {
            value = std::make_shared<T>();
        }
        JsonAdapter<T>::deserialize(json, *value);
    }
};

/**
 * @brief JSON handling for std::optional<T>.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<std::optional<T>> : PtrAdapter<std::optional<T>>
{
    static void deserialize(const JsonValue &json, std::optional<T> &value)
    {
        if (json.isEmpty())
        {
            value = std::nullopt;
            return;
        }
        if (!value)
        {
            value.emplace();
        }
        JsonAdapter<T>::deserialize(json, *value);
    }
};
} // namespace brayns

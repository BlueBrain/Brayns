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

#include <memory>
#include <optional>

#include "PrimitiveAdapter.h"

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
    /**
     * @brief Create a JSON schema based on the value referenced by T.
     *
     * @return JsonSchema JSON schema of *T.
     */
    static JsonSchema getSchema()
    {
        using ValueType = std::decay_t<decltype(*T{})>;
        return Json::getSchema<ValueType>();
    }

    /**
     * @brief Serialize contained value if not null.
     *
     * @param value Input value.
     * @param json Output JSON.
     */
    static void serialize(const T &value, JsonValue &json)
    {
        if (!value)
        {
            json.clear();
            return;
        }
        Json::serialize(*value, json);
    }

    /**
     * @brief Deserialize contained value if not null.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, T &value)
    {
        if (!value)
        {
            return;
        }
        if (json.isEmpty())
        {
            value = {};
            return;
        }
        Json::deserialize(json, *value);
    }
};

/**
 * @brief Allow JSON handling for pointer types.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<T *> : PtrAdapter<T *>
{
};

/**
 * @brief Allow JSON handling for std::unique_ptr.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<std::unique_ptr<T>> : PtrAdapter<std::unique_ptr<T>>
{
    /**
     * @brief Create value using make_unique<T>() if empty.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, std::unique_ptr<T> &value)
    {
        if (!value)
        {
            value = std::make_unique<T>();
        }
        Json::deserialize(json, *value);
    }
};

/**
 * @brief Allow JSON handling for std::shared_ptr.
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
        if (!value)
        {
            value = std::make_shared<T>();
        }
        Json::deserialize(json, *value);
    }
};

/**
 * @brief Allow JSON handling for std::optional.
 *
 * @tparam T Referenced type.
 */
template<typename T>
struct JsonAdapter<std::optional<T>> : PtrAdapter<std::optional<T>>
{
    /**
     * @brief Emplace object using value.emplace() if empty.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, std::optional<T> &value)
    {
        if (!value)
        {
            value.emplace();
        }
        Json::deserialize(json, *value);
    }
};
} // namespace brayns

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

#include <memory>

#include <boost/optional.hpp>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper for JsonAdapter for pointer-like values.
 *
 * @tparam T Contained type.
 */
template <typename T>
struct PtrAdapter
{
    /**
     * @brief Create a JSON schema based on the value referenced by T.
     *
     * @return JsonSchema JSON schema of *T.
     */
    static JsonSchema getSchema(const T& value)
    {
        if (!value)
        {
            return Json::getSchema<std::decay_t<decltype(*value)>>();
        }
        return Json::getSchema(*value);
    }

    /**
     * @brief Serialize contained value if not null.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @return true Success.
     * @return false Failure.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        if (!value)
        {
            return false;
        }
        return Json::serialize(*value, json);
    }

    /**
     * @brief Deserialize contained value if not null.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        if (!value)
        {
            return false;
        }
        return Json::deserialize(json, *value);
    }
};

/**
 * @brief Allow JSON handling for pointer types.
 *
 * @tparam T Referenced type.
 */
template <typename T>
struct JsonAdapter<T*> : PtrAdapter<T*>
{
};

/**
 * @brief Allow JSON handling for std::unique_ptr
 *
 * @tparam T Referenced type.
 */
template <typename T>
struct JsonAdapter<std::unique_ptr<T>> : PtrAdapter<std::unique_ptr<T>>
{
    /**
     * @brief Deserialize using std::make_unique<T> and JsonAdapter<T>.
     *
     * @param json Input JSON.
     * @param value Ouput value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, std::unique_ptr<T>& value)
    {
        T buffer = {};
        if (!Json::deserialize(json, buffer))
        {
            return false;
        }
        value = std::make_unique<T>(std::move(buffer));
        return true;
    }
};

/**
 * @brief Allow JSON handling for std::shared_ptr
 *
 * @tparam T Referenced type.
 */
template <typename T>
struct JsonAdapter<std::shared_ptr<T>> : PtrAdapter<std::shared_ptr<T>>
{
    /**
     * @brief Deserialize using std::make_shared<T> and JsonAdapter<T>.
     *
     * @param json Input JSON.
     * @param value Ouput value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, std::shared_ptr<T>& value)
    {
        T buffer = {};
        if (!Json::deserialize(json, buffer))
        {
            return false;
        }
        value = std::make_shared<T>(std::move(buffer));
        return true;
    }
};

/**
 * @brief Allow JSON handling for boost::optional
 *
 * @tparam T Referenced type.
 */
template <typename T>
struct JsonAdapter<boost::optional<T>> : PtrAdapter<boost::optional<T>>
{
    /**
     * @brief Deserialize and fill an optional value of T using JsonAdapter<T>.
     *
     * The output value will be left unchanged if fails.
     *
     * @param json Input JSON.
     * @param value Ouput value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, boost::optional<T>& value)
    {
        T buffer = {};
        if (!Json::deserialize(json, buffer))
        {
            return false;
        }
        value = std::move(buffer);
        return true;
    }
};
} // namespace brayns
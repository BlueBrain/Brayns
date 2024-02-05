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

#include <brayns/common/MathTypes.h>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to serialize GLM types to a JSON array.
 *
 * @tparam T GLM type.
 */
template <typename T>
struct GlmAdapter
{
    using ValueType = typename T::value_type;

    /**
     * @brief Create a JSON schema with type array, item type of the reflected
     * GLM type items and min and max size of the GLM type size.
     *
     * @return JsonSchema JSON schema of T.
     */
    static JsonSchema getSchema(const T&)
    {
        JsonSchema schema;
        schema.type = JsonType::Array;
        schema.items = {Json::getSchema<ValueType>()};
        schema.minItems = size_t(T::length());
        schema.maxItems = size_t(T::length());
        return schema;
    }

    /**
     * @brief Serialize value in json.
     *
     * Derialize all elements of the GLM type using
     * JsonAdapter<T::value_type>::serialize and put the array inside the
     * provided JsonValue. If it fails, json is left unchanged.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @return true Success.
     * @return false Failure.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (glm::length_t i = 0; i < T::length(); ++i)
        {
            JsonValue jsonItem;
            Json::serialize(value[i], jsonItem);
            array->add(jsonItem);
        }
        json = array;
        return true;
    }

    /**
     * @brief Deserialize json in value.
     *
     * Deserialize all elements in the provided GLM type using
     * JsonAdapter<T::value_type>::deserialize. If the json is not a
     * JsonArray::Ptr, the value is left unchanged, if its size is not S, only
     * the common indices will be updated in min(T::length(), array.size()).
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        auto array = JsonExtractor::extractArray(json);
        if (!array)
        {
            return false;
        }
        auto size = std::min(T::length(), glm::length_t(array->size()));
        for (glm::length_t i = 0; i < size; ++i)
        {
            Json::deserialize(array->get(i), value[i]);
        }
        return true;
    }
};

/**
 * @brief Partial specialization of JsonAdapter for glm::vec<S, T>.
 *
 * @tparam S Size of the vector.
 * @tparam T Type of the vector components.
 */
template <glm::length_t S, typename T>
struct JsonAdapter<glm::vec<S, T>> : GlmAdapter<glm::vec<S, T>>
{
};

/**
 * @brief Partial specialization of JsonAdapter for glm::qua<T>.
 *
 * @tparam T Type of the quaternion components.
 */
template <typename T>
struct JsonAdapter<glm::qua<T>> : GlmAdapter<glm::qua<T>>
{
};
} // namespace brayns
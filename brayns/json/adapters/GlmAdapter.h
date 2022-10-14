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

#include <brayns/utils/MathTypes.h>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to serialize GLM types to a JSON array.
 *
 * @tparam T GLM type.
 */
template<typename T>
struct GlmAdapter
{
    using ValueType = typename T::value_type;

    /**
     * @brief Create a JSON schema with type array, item type of the reflected
     * GLM type items and min and max size of the GLM type size.
     *
     * @return JsonSchema JSON schema of T.
     */
    static JsonSchema getSchema()
    {
        JsonSchema schema;
        schema.type = JsonType::Array;
        schema.items = {Json::getSchema<ValueType>()};
        schema.minItems = static_cast<size_t>(T::length());
        schema.maxItems = static_cast<size_t>(T::length());
        return schema;
    }

    /**
     * @brief Serialize value in json.
     *
     * Derialize all elements of the GLM type using JsonAdapter<T::value_type>.
     *
     * @param value Input value.
     * @param json Output JSON.
     */
    static void serialize(const T &value, JsonValue &json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (glm::length_t i = 0; i < T::length(); ++i)
        {
            auto jsonItem = Json::serialize(value[i]);
            array->add(jsonItem);
        }
        json = array;
    }

    /**
     * @brief Deserialize json in value.
     *
     * Deserialize all elements using JsonAdapter<T::value_type>.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, T &value)
    {
        auto array = JsonExtractor::extractArray(json);
        if (!array)
        {
            throw std::runtime_error("Not a JSON array");
        }
        auto size = static_cast<glm::length_t>(array->size());
        if (size != T::length())
        {
            throw std::runtime_error("Invalid array size");
        }
        glm::length_t i = 0;
        for (const auto &item : *array)
        {
            Json::deserialize(item, value[i++]);
        }
    }
};

/**
 * @brief Partial specialization of JsonAdapter for glm::vec<S, T>.
 *
 * @tparam S Size of the vector.
 * @tparam T Type of the vector components.
 */
template<glm::length_t S, typename T>
struct JsonAdapter<glm::vec<S, T>> : GlmAdapter<glm::vec<S, T>>
{
};

/**
 * @brief Partial specialization of JsonAdapter for glm::qua<T>.
 *
 * @tparam T Type of the quaternion components.
 */
template<typename T>
struct JsonAdapter<glm::qua<T>> : GlmAdapter<glm::qua<T>>
{
};
} // namespace brayns

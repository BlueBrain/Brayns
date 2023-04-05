/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <stdexcept>

#include <brayns/utils/MathTypes.h>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to serialize GLM types to fixed-size JSON array.
 *
 * @tparam T GLM vector type.
 */
template<typename T>
struct GlmAdapter
{
    using ValueType = typename T::value_type;

    static const auto itemCount = static_cast<size_t>(T::length());

    static JsonSchema getSchema()
    {
        auto schema = JsonSchema();
        schema.type = JsonType::Array;
        schema.items = {JsonAdapter<ValueType>::getSchema()};
        schema.minItems = itemCount;
        schema.maxItems = itemCount;
        return schema;
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto &array = JsonFactory::emplaceArray(json);
        for (glm::length_t i = 0; i < T::length(); ++i)
        {
            auto child = JsonValue();
            JsonAdapter<ValueType>::serialize(value[i], child);
            array.add(child);
        }
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        auto &array = JsonExtractor::extractArray(json);
        if (array.size() != itemCount)
        {
            throw std::invalid_argument("Invalid array size");
        }
        auto i = glm::length_t(0);
        for (const auto &item : array)
        {
            JsonAdapter<ValueType>::deserialize(item, value[i]);
            ++i;
        }
    }
};

/**
 * @brief JSON handling for glm::vec<S, T>.
 *
 * @tparam S Component count.
 * @tparam T Component type.
 */
template<glm::length_t S, typename T>
struct JsonAdapter<glm::vec<S, T>> : GlmAdapter<glm::vec<S, T>>
{
};

/**
 * @brief JSON handling for glm::qua<T>.
 *
 * @tparam T Component type.
 */
template<typename T>
struct JsonAdapter<glm::qua<T>> : GlmAdapter<glm::qua<T>>
{
};
} // namespace brayns

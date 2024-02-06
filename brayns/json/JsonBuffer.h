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

#include "JsonAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to delay the serialization of a JSON.
 *
 * Has the same schema than T but store the JSON value internally when
 * serializing with no JSON calls.
 *
 * Can be used instead of an instance of T when serialization must not happen
 * immediately. The serialization can then be done manually when required.
 *
 * Example:
 * @code {.cpp}
 * BRAYNS_JSON_OBJECT_BEGIN(Test)
 * BRAYNS_JSON_OBJECT_ENTRY(int, now, "Processed immediately")
 * BRAYNS_JSON_OBJECT_ENTRY(JsonBuffer<int>, later, "Delay JSON processing")
 * BRAYNS_JSON_OBJECT_END()
 * @endcode
 *
 * In the example when we call Json::deserialize(json, test), the JSON at key
 * "now" is stored inside test.now while the value at "later" is (de)serialized
 * directly but now appears as an int in the JSON schema of Test.
 *
 * @tparam T Type to wrap.
 */
template<typename T>
class JsonBuffer
{
public:
    /**
     * @brief Return the JSON schema of T.
     *
     * @return JsonSchema JSON schema of T.
     */
    static JsonSchema getSchema()
    {
        return JsonAdapter<T>::getSchema();
    }

    /**
     * @brief Copy the internally stored JSON into parameter.
     *
     * @param json Output JSON.
     */
    void serialize(JsonValue &json) const
    {
        json = _json;
    }

    /**
     * @brief Store JSON internally.
     *
     * @param json Input JSON.
     */
    void deserialize(const JsonValue &json)
    {
        _json = json;
    }

    /**
     * @brief Deserialize stored json into value.
     *
     * Called manually when required.
     *
     * @param value Output value.
     */
    void extract(T &value) const
    {
        JsonAdapter<T>::deserialize(_json, value);
    }

private:
    JsonValue _json;
};
} // namespace brayns

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

#include "Json.h"

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
 * @tparam T Type to serialize.
 */
template <typename T>
class JsonBuffer
{
public:
    /**
     * @brief Return the JSON schema of T.
     *
     * @return JsonSchema JSON schema of T.
     */
    JsonSchema getSchema() const { return Json::getSchema<T>(); }

    /**
     * @brief Copy the internally stored JSON into parameter.
     *
     * @param json Output JSON.
     * @return true Always.
     * @return false Never.
     */
    bool serialize(JsonValue& json) const
    {
        json = _json;
        return true;
    }

    /**
     * @brief Store JSON internally.
     *
     * @param json Input JSON.
     * @return true Always.
     * @return false Never.
     */
    bool deserialize(const JsonValue& json)
    {
        _json = json;
        return true;
    }

    /**
     * @brief Deserialize internal json to value.
     *
     * Called manually when required.
     *
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    bool deserialize(T& value) const { return Json::deserialize(_json, value); }

    /**
     * @brief Get the internal JSON value.
     *
     * @return const JsonValue& Internal JSON.
     */
    const JsonValue& getJson() const { return _json; }

private:
    JsonValue _json;
};
} // namespace brayns
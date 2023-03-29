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

#include <functional>
#include <string>

#include <brayns/json/JsonAdapter.h>

namespace brayns
{
struct JsonProperty
{
    std::string name;
    JsonSchema schema;
    bool required = false;
    std::function<void(const void *, JsonValue &)> serialize;
    std::function<void(const JsonValue &, void *)> deserialize;
};

struct JsonObjectInfo
{
    std::string title;
    std::vector<JsonProperty> properties;
};

class JsonObjectHelper
{
public:
    static JsonSchema getSchema(const JsonObjectInfo &object);
    static void serialize(const JsonObjectInfo &object, const void *value, JsonValue &json);
    static void deserialize(const JsonObjectInfo &object, const JsonValue &json, void *value);
};

template<typename T>
struct ObjectAdapter
{
    static JsonSchema getSchema()
    {
        return JsonObjectHelper::getSchema(_object);
    }

    static void serialize(const T &value, JsonValue &json)
    {
        JsonObjectHelper::serialize(_object, value, json);
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        JsonObjectHelper::deserialize(_object, json, value);
    }

private:
    static inline JsonObjectInfo _object;
};
} // namespace brayns

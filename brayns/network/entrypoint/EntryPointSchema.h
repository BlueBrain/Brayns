/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <string>
#include <vector>

#include <brayns/network/messages/JsonSchema.h>

namespace brayns
{
struct EntryPointSchema
{
    std::string title;
    std::string description;
    std::string type = "method";
    bool async = false;
    std::vector<JsonSchema> params;
    JsonSchema returns;
};

template <>
struct JsonSerializer<EntryPointSchema>
{
    static bool serialize(const EntryPointSchema& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        object->set("title", value.title);
        object->set("description", value.description);
        object->set("type", value.type);
        object->set("async", value.async);
        object->set("params", Json::serialize(value.params));
        object->set("returns", Json::serialize(value.returns));
        json = object;
        return true;
    }

    static bool deserialize(const JsonValue& json, EntryPointSchema& value)
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            return false;
        }
        Json::deserialize(object->get("title"), value.title);
        Json::deserialize(object->get("description"), value.description);
        Json::deserialize(object->get("type"), value.type);
        Json::deserialize(object->get("async"), value.async);
        Json::deserialize(object->get("params"), value.params);
        Json::deserialize(object->get("returns"), value.returns);
        return true;
    }
};
} // namespace brayns
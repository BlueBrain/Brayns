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

#include <brayns/network/messages/JsonSchema.h>

namespace brayns
{
struct EntryPointSchema
{
    JsonSchema params;
    JsonSchema returns;

    template<typename RequestType, typename ReplyType>
    static EntryPointSchema from()
    {
        EntryPointSchema schema;
        schema.params = JsonSchemaFactory<RequestType>::createJsonSchema();
        schema.returns = JsonSchemaFactory<ReplyType>::createJsonSchema();
        return schema;
    }
};

template<>
struct JsonSerializer<EntryPointSchema>
{
    static void serialize(const EntryPointSchema& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        if (!value.params.isEmpty())
        {
            object->set("params", Json::serialize(value.params));
        }
        if (!value.returns.isEmpty())
        {
            object->set("returns", Json::serialize(value.returns));
        }
        json = object;
    }

    static void deserialize(const JsonValue& json, EntryPointSchema& value)
    {
        if (json.type() != typeid(JsonObject::Ptr))
        {
            return;
        }
        auto& object = *json.extract<JsonObject::Ptr>();
        Json::deserialize(object.get("params"), value.params);
        Json::deserialize(object.get("returns"), value.returns);
    }
};
} // namespace brayns
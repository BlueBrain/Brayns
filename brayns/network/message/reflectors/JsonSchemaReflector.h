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

#include "ArrayReflector.h"
#include "MapReflector.h"
#include "PtrReflector.h"
#include "PrimitiveReflector.h"

namespace brayns
{
template <>
struct JsonReflector<JsonSchema>
{
    static JsonSchema getSchema(const JsonSchema&) { return {}; }

    static bool serialize(const JsonSchema& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        JsonHelper::setIfNotEmpty(*object, "title", value.title);
        JsonHelper::setIfNotEmpty(*object, "description", value.description);
        JsonHelper::setIfNotEmpty(*object, "type", value.type);
        JsonHelper::setIfNotEmpty(*object, "oneOf", value.oneOf);
        JsonHelper::setIfNotNull(*object, "minimum", value.minimum);
        JsonHelper::setIfNotNull(*object, "maximum", value.maximum);
        JsonHelper::setIfNotEmpty(*object, "properties", value.properties);
        JsonHelper::setIfNotEmpty(*object, "required", value.required);
        JsonHelper::setFirstIfNotEmpty(*object, "additionalProperties",
                                       value.additionalProperties);
        JsonHelper::setFirstIfNotEmpty(*object, "items", value.items);
        JsonHelper::setIfNotNull(*object, "minItems", value.minItems);
        JsonHelper::setIfNotNull(*object, "maxItems", value.maxItems);
        json = object;
        return true;
    }

    static bool deserialize(const JsonValue& json, JsonSchema& value)
    {
        auto object = JsonHelper::extractObject(json);
        if (!object)
        {
            return false;
        }
        Json::deserialize(object->get("title"), value.title);
        Json::deserialize(object->get("description"), value.description);
        Json::deserialize(object->get("type"), value.type);
        Json::deserialize(object->get("oneOf"), value.oneOf);
        Json::deserialize(object->get("minimum"), value.minimum);
        Json::deserialize(object->get("maximum"), value.maximum);
        Json::deserialize(object->get("properties"), value.properties);
        Json::deserialize(object->get("required"), value.required);
        Json::deserialize(object->get("additionalProperties"),
                          value.additionalProperties);
        Json::deserialize(object->get("items"), value.items);
        Json::deserialize(object->get("minItems"), value.minItems);
        Json::deserialize(object->get("maxItems"), value.maxItems);
        return true;
    }
};
} // namespace brayns
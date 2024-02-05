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

#include <brayns/common/PropertyObject.h>

#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
/**
 * @brief Helper class to create a JSON schema from a PropertyObjet.
 *
 */
class PropertyObjectSchema
{
public:
    /**
     * @brief Create a oneOf schema from a property object.
     *
     * @param object Object to build the schema from.
     * @return JsonSchema Schema of object.
     */
    static JsonSchema create(const PropertyObject& object)
    {
        JsonSchema schema;
        auto& oneOf = schema.oneOf;
        auto& properties = object.getProperties();
        oneOf.reserve(properties.size());
        for (const auto& pair : properties)
        {
            oneOf.push_back(_getSchema(pair));
        }
        return schema;
    }

private:
    static JsonSchema _getSchema(
        const std::pair<std::string, PropertyMap>& pair)
    {
        auto& properties = pair.second;
        auto schema = Json::getSchema(properties);
        auto& type = pair.first;
        schema.title = type;
        return schema;
    }
};
} // namespace brayns

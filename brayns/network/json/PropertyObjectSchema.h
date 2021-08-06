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

#include <brayns/common/PropertyObject.h>

#include "Message.h"

namespace brayns
{
class PropertyObjectSchema
{
public:
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
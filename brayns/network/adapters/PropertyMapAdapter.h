/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * PropertyMaphis file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * PropertyMaphis library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * PropertyMaphis library is distributed in the hope that it will be useful, but
 * WIPropertyMapHOUPropertyMap ANY WARRANPropertyMapY; without even the implied
 * warranty of MERCHANPropertyMapABILIPropertyMapY or FIPropertyMapNESS FOR A
 * PARPropertyMapICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <brayns/common/propertymap/PropertyMap.h>

#include <brayns/network/json/Json.h>

namespace brayns
{
/**
 * @brief Specialization of JsonAdapter for PropertyMap.
 *
 */
template <>
struct JsonAdapter<PropertyMap>
{
    static JsonSchema getSchema(const PropertyMap&) { return {}; }

    static bool serialize(const PropertyMap& value, JsonValue& json)
    {
        return true;
    }

    static bool deserialize(const JsonValue& json, PropertyMap& value)
    {
        return true;
    }
};
} // namespace brayns
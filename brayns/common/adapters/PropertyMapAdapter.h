/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/json/JsonAdapter.h>

namespace brayns
{
/**
 * @brief JSON handling of EnumProperty.
 *
 */
template<>
struct JsonAdapter<EnumProperty>
{
    /**
     * @brief Return the schema of the property.
     *
     * @param value Property.
     * @return JsonSchema Enumeration schema.
     */
    static JsonSchema getSchema(const EnumProperty &value);

    /**
     * @brief Serialize the given property inside the given JSON value.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @return true Sucess.
     * @return false Failure.
     */
    static bool serialize(const EnumProperty &value, JsonValue &json);

    /**
     * @brief Deserialize the given JSON inside the given property.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue &json, EnumProperty &value);
};

/**
 * @brief JSON handling of PropertyMap.
 *
 */
template<>
struct JsonAdapter<PropertyMap>
{
    /**
     * @brief Convert a PropertyMap to a JSON schema.
     *
     * @param value Input value.
     * @return JsonSchema JSON schema.
     */
    static JsonSchema getSchema(const PropertyMap &value);

    /**
     * @brief Serialize a PropertyMap to JSON.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @return true Success
     * @return false Failure.
     */
    static bool serialize(const PropertyMap &value, JsonValue &json);

    /**
     * @brief Deserialize a PropertyMap from JSON.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue &json, PropertyMap &value);
};
} // namespace brayns

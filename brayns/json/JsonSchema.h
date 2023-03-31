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

#include <map>
#include <string>
#include <vector>

#include "JsonType.h"

namespace brayns
{
/**
 * @brief Representation of a JSON schema.
 *
 */
struct JsonSchema
{
    /**
     * @brief Title of the schema.
     *
     */
    std::string title;

    /**
     * @brief Schema description.
     *
     */
    std::string description;

    /**
     * @brief Cannot be omitted if inside an object.
     *
     */
    bool required = false;

    /**
     * @brief Cannot be written if true.
     *
     */
    bool readOnly = false;

    /**
     * @brief Cannot be read if true.
     *
     */
    bool writeOnly = false;

    /**
     * @brief Default value.
     *
     */
    JsonValue defaultValue;

    /**
     * @brief JSON type if not a union.
     *
     */
    JsonType type = JsonType::Undefined;

    /**
     * @brief Optional min value if integer or number.
     *
     */
    double minimum = 0.0;

    /**
     * @brief Optional max value if integer or number.
     *
     */
    double maximum = 0.0;

    /**
     * @brief Holds one schema of items if array or map, else empty.
     *
     */
    std::vector<JsonSchema> items;

    /**
     * @brief Optional min item count if array.
     *
     */
    size_t minItems = 0;

    /**
     * @brief Optional max item count if array.
     *
     */
    size_t maxItems = 0;

    /**
     * @brief List of object properties if object, else empty.
     *
     */
    std::map<std::string, JsonSchema> properties;

    /**
     * @brief Enum description, empty if not an enum.
     *
     */
    std::vector<std::string> enums;

    /**
     * @brief Union description, empty if not an union.
     *
     */
    std::vector<JsonSchema> oneOf;
};
} // namespace brayns

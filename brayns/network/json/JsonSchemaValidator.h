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

#include <brayns/common/utils/stringUtils.h>

#include "Json.h"

namespace brayns
{
/**
 * @brief Helper class to store JSON schema validation errors.
 *
 */
class JsonSchemaErrorList
{
public:
    JsonSchemaErrorList() = default;

    JsonSchemaErrorList(std::vector<std::string> errors)
        : _errors(std::move(errors))
    {
    }

    auto begin() const { return _errors.begin(); }

    auto end() const { return _errors.end(); }

    bool isEmpty() const { return _errors.empty(); }

    const std::vector<std::string>& asStringList() const { return _errors; }

private:
    std::vector<std::string> _errors;
};

/**
 * @brief Validate a JSON value using a JSON schema.
 *
 */
class JsonSchemaValidator
{
public:
    static JsonSchemaErrorList validate(const JsonValue& json,
                                        const JsonSchema& schema);
};
} // namespace brayns
/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <string>
#include <variant>
#include <vector>

#include "JsonSchema.h"
#include "JsonValue.h"

namespace brayns
{
using JsonPathItem = std::variant<std::size_t, std::string>;
using JsonPath = std::vector<JsonPathItem>;

std::string toString(const JsonPath &path);

struct InvalidType
{
    JsonType type;
    JsonType expected;
};

std::string toString(const InvalidType &error);

struct InvalidConst
{
    std::string value;
    std::string expected;
};

std::string toString(const InvalidConst &error);

struct BelowMinimum
{
    double value;
    double minimum;
};

std::string toString(const BelowMinimum &error);

struct AboveMaximum
{
    double value;
    double maximum;
};

std::string toString(const AboveMaximum &error);

struct DuplicatedItem
{
    std::string value;
};

std::string toString(const DuplicatedItem &error);

struct NotEnoughItems
{
    std::size_t count;
    std::size_t minItems;
};

std::string toString(const NotEnoughItems &error);

struct TooManyItems
{
    std::size_t count;
    std::size_t maxItems;
};

std::string toString(const TooManyItems &error);

struct MissingRequiredProperty
{
    std::string name;
};

std::string toString(const MissingRequiredProperty &error);

struct UnknownProperty
{
    std::string name;
};

std::string toString(const UnknownProperty &error);

struct InvalidOneOf
{
    JsonValue value;
};

std::string toString(const InvalidOneOf &error);

using JsonError = std::variant<
    InvalidType,
    InvalidConst,
    AboveMaximum,
    BelowMinimum,
    DuplicatedItem,
    TooManyItems,
    NotEnoughItems,
    MissingRequiredProperty,
    UnknownProperty,
    InvalidOneOf>;

std::string toString(const JsonError &error);

struct JsonSchemaError
{
    JsonPath path;
    JsonError error;
};

std::vector<JsonSchemaError> validate(const JsonValue &json, const JsonSchema &schema);
}

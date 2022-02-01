/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "JsonOptions.h"

namespace
{
class OptionalHelper
{
public:
    template<typename T, typename U>
    static void trySet(T &value, const U &newValue)
    {
        if (!newValue)
        {
            return;
        }
        value = *newValue;
    }
};
}

namespace brayns
{
void JsonOptions::build(JsonOptions &options)
{
    (void)options;
}

void JsonSchemaOptions::add(JsonSchema &schema, const JsonOptions &options)
{
    OptionalHelper::trySet(schema.title, options.title);
    OptionalHelper::trySet(schema.description, options.description);
    OptionalHelper::trySet(schema.readOnly, options.readOnly);
    OptionalHelper::trySet(schema.writeOnly, options.writeOnly);
    OptionalHelper::trySet(schema.minimum, options.minimum);
    OptionalHelper::trySet(schema.maximum, options.maximum);
    OptionalHelper::trySet(schema.minItems, options.minItems);
    OptionalHelper::trySet(schema.maxItems, options.maxItems);
    OptionalHelper::trySet(schema.defaultValue, options.defaultValue);
}

Title::Title(std::string title)
    : value(std::move(title))
{
}

void Title::add(JsonOptions &options) const
{
    options.title = value;
}

Description::Description(std::string description)
    : value(std::move(description))
{
}

void Description::add(JsonOptions &options) const
{
    options.description = value;
}

Required::Required(bool required)
    : value(required)
{
}

void Required::add(JsonOptions &options) const
{
    options.required = value;
}

ReadOnly::ReadOnly(bool readOnly)
    : value(readOnly)
{
}

void ReadOnly::add(JsonOptions &options) const
{
    options.readOnly = value;
}

WriteOnly::WriteOnly(bool writeOnly)
    : value(writeOnly)
{
}

void WriteOnly::add(JsonOptions &options) const
{
    options.writeOnly = value;
}

Minimum::Minimum(double minimum)
    : value(minimum)
{
}

void Minimum::add(JsonOptions &options) const
{
    options.minimum = value;
}

Maximum::Maximum(double maximum)
    : value(maximum)
{
}

void Maximum::add(JsonOptions &options) const
{
    options.maximum = value;
}

MinItems::MinItems(size_t minItems)
    : value(minItems)
{
}

void MinItems::add(JsonOptions &options) const
{
    options.minItems = value;
}

MaxItems::MaxItems(size_t maxItems)
    : value(maxItems)
{
}

void MaxItems::add(JsonOptions &options) const
{
    options.maxItems = value;
}

Default::Default(const JsonValue &defaultValue)
    : value(defaultValue)
{
}

void Default::add(JsonOptions &options) const
{
    options.defaultValue = value;
    options.required = false;
}
} // namespace brayns

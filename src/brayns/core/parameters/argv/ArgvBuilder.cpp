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

#include "ArgvBuilder.h"

#include <cassert>

namespace brayns
{
ArgvBuilder::ArgvBuilder(std::vector<ArgvProperty> &properties):
    _properties(properties)
{
}

ArgvBuilder &ArgvBuilder::multitoken(bool value)
{
    assert(!_properties.empty());
    auto &property = _properties.back();
    property.multitoken = value;
    property.composable = !value;
    return *this;
}

ArgvBuilder &ArgvBuilder::composable(bool value)
{
    assert(!_properties.empty());
    auto &property = _properties.back();
    property.composable = value;
    property.multitoken = !value;
    return *this;
}

ArgvBuilder &ArgvBuilder::minimum(std::optional<double> value)
{
    assert(!_properties.empty());
    auto &property = _properties.back();
    assert(property.type == ArgvType::Integer || property.type == ArgvType::Number);
    property.minimum = value;
    return *this;
}

ArgvBuilder &ArgvBuilder::maximum(std::optional<double> value)
{
    assert(!_properties.empty());
    auto &property = _properties.back();
    assert(property.type == ArgvType::Integer || property.type == ArgvType::Number);
    property.maximum = value;
    return *this;
}

ArgvBuilder &ArgvBuilder::minItems(std::optional<size_t> value)
{
    assert(!_properties.empty());
    auto &property = _properties.back();
    assert(property.multitoken || property.composable);
    property.minItems = value;
    return *this;
}

ArgvBuilder &ArgvBuilder::maxItems(std::optional<size_t> value)
{
    assert(!_properties.empty());
    auto &property = _properties.back();
    assert(property.multitoken || property.composable);
    property.maxItems = value;
    return *this;
}

ArgvBuilder &ArgvBuilder::itemCount(std::optional<size_t> value)
{
    minItems(value);
    return maxItems(value);
}
} // namespace brayns

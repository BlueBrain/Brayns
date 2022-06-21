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

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "ArgvProperty.h"
#include "ArgvReflector.h"

namespace brayns
{
class ArgvBuilder
{
public:
    ArgvBuilder(std::vector<ArgvProperty> &properties);

    ArgvBuilder &multitoken(bool value = true);
    ArgvBuilder &composable(bool value = true);
    ArgvBuilder &minimum(std::optional<double> value);
    ArgvBuilder &maximum(std::optional<double> value);
    ArgvBuilder &between(double minValue, double maxValue);
    ArgvBuilder &minItems(std::optional<size_t> value);
    ArgvBuilder &maxItems(std::optional<size_t> value);
    ArgvBuilder &itemCount(std::optional<size_t> value);

    template<typename T>
    ArgvBuilder &add(std::string name, T &value, std::string description = {})
    {
        auto property = ArgvReflector<T>::reflect(value);
        property.name = std::move(name);
        property.description = std::move(description);
        _properties.push_back(std::move(property));
        return *this;
    }

private:
    std::vector<ArgvProperty> &_properties;
};
} // namespace brayns

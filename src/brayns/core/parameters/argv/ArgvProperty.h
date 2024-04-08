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

#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "ArgvValue.h"

namespace brayns
{
struct ArgvProperty
{
    ArgvType type = ArgvType::String;
    std::string name;
    std::string description;
    std::function<void(const std::vector<ArgvValue> &)> load;
    std::function<std::string()> stringify;
    bool multitoken = false;
    bool composable = false;
    std::vector<std::string> enums;
    std::optional<double> minimum;
    std::optional<double> maximum;
    std::optional<size_t> minItems;
    std::optional<size_t> maxItems;
};

struct GetArgvProperty
{
    template<typename T>
    static ArgvProperty of()
    {
        ArgvProperty property;
        property.type = ArgvTypeInfo::getType<T>();
        if constexpr (ArgvTypeInfo::isNumeric<T>())
        {
            property.minimum = std::numeric_limits<T>::lowest();
            property.maximum = std::numeric_limits<T>::max();
        }
        if constexpr (std::is_enum_v<T>)
        {
            property.enums = EnumInfo::getNames<T>();
        }
        return property;
    }
};
} // namespace brayns

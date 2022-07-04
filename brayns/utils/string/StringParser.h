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

#include <limits>
#include <string_view>
#include <type_traits>

namespace brayns
{
class StringParserHelper
{
public:
    static double parse(std::string_view data);
    static void checkLimits(double value, double min, double max);
    static void checkIsInteger(double value);
};

template<typename T>
struct StringParser
{
    static T parse(std::string_view data)
    {
        auto value = StringParserHelper::parse(data);
        auto min = static_cast<double>(std::numeric_limits<T>::min());
        auto max = static_cast<double>(std::numeric_limits<T>::max());
        StringParserHelper::checkLimits(value, min, max);
        if constexpr (std::is_integral_v<T>)
        {
            StringParserHelper::checkIsInteger(value);
        }
        return static_cast<T>(value);
    }
};

template<>
struct StringParser<bool>
{
    static bool parse(std::string_view data);
};
} // namespace brayns

/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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
#include <tuple>
#include <vector>

namespace brayns
{
/*
 * This file contains helper functions for converting strings to and from enums.
 * To use the helper functions define the method 'enumMap' with a specialization
 * for your enum type.
 */

/* Returns a mapping from a name to an enum type. */
template <typename EnumT>
std::vector<std::pair<std::string, EnumT>> enumMap();

/* Returns all names for given enum type 'EnumT' */
template <typename EnumT>
inline std::vector<std::string> enumNames()
{
    std::vector<std::string> v;
    for (const auto& p : enumMap<EnumT>())
        v.push_back(p.first);
    return v;
}

/* Convert a string to an enum. */
template <typename EnumT>
inline EnumT stringToEnum(const std::string& v)
{
    for (const auto& p : enumMap<EnumT>())
        if (p.first == v)
            return p.second;

    throw std::runtime_error("Could not match enum '" + v + "'");
    return static_cast<EnumT>(0);
}

/* Convert an enum to a string. */
template <typename EnumT>
inline std::string enumToString(const EnumT v)
{
    for (const auto& p : enumMap<EnumT>())
        if (p.second == v)
            return p.first;

    throw std::runtime_error("Could not match enum");
    return "Invalid";
}
} // namespace brayns

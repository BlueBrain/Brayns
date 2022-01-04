/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <stdexcept>
#include <string_view>

#include "EndianHelper.h"

namespace brayns
{
class BinaryHelper
{
public:
    template <typename T>
    static T extractBigEndian(std::string_view &line)
    {
        auto value = extract<T>(line);
        return EndianHelper::convertBigEndianToLocalEndian(value);
    }

    template <typename T>
    static T extractLittleEndian(std::string_view &line)
    {
        auto value = extract<T>(line);
        return EndianHelper::convertLittleEndianToLocalEndian(value);
    }

    template <typename T>
    static T extract(std::string_view &line)
    {
        auto stride = sizeof(T);
        if (line.size() < stride)
        {
            throw std::runtime_error("Line too short");
        }
        T value;
        std::memcpy(&value, line.data(), stride);
        line = line.substr(stride);
        return value;
    }
};
} // namespace brayns

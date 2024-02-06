/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <cstdint>
#include <vector>

class DataMinMax
{
public:
    template<typename T>
    static std::pair<T, T> compute(const std::vector<T> &input)
    {
        T min = std::numeric_limits<T>::max();
        T max = std::numeric_limits<T>::lowest();

        for (size_t i = 0; i < input.size(); ++i)
        {
            min = std::min(input[i], min);
            max = std::max(input[i], max);
        }

        return std::make_pair(min, max);
    }
};

class DataToBytes
{
public:
    template<typename T>
    static std::vector<uint8_t> convert(const std::vector<T> &input)
    {
        std::vector<uint8_t> result(input.size() * sizeof(T));
        const auto src = input.data();
        auto dst = result.data();
        std::memcpy(dst, src, result.size());
        return result;
    }
};

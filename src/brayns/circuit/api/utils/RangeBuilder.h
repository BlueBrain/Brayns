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

#include <brayns/core/utils/string/StringJoiner.h>

#include <stdexcept>
#include <string>
#include <vector>

template<typename T>
class RangeBuilder
{
public:
    /**
     * @brief Builds a range string from a list of integral numbers. It is assumed that the provided list is sorted.
     *
     * @param input list of integers, must be sorted in ascending order.
     * @return std::string Resulting range.
     */
    static std::string build(const std::vector<T> &input)
    {
        static_assert(std::is_integral_v<T>, "Only integral types allowed");

        if (input.empty())
        {
            throw std::invalid_argument("Input integer list cannot be empty");
        }

        auto rangeList = _buildRangeList(input);
        return brayns::StringJoiner::join(rangeList, ",");
    }

private:
    static size_t _countRanges(const std::vector<T> &input)
    {
        size_t ranges = 1;

        for (size_t i = 1; i < input.size(); ++i)
        {
            if (input[i] != input[i - 1] + 1)
            {
                ++ranges;
            }
        }

        return ranges;
    }

    static std::string _buildRange(T start, T end)
    {
        if (start == end)
        {
            return std::to_string(start);
        }

        return std::to_string(start) + "-" + std::to_string(end);
    }

    static std::vector<std::string> _buildRangeList(const std::vector<T> &input)
    {
        auto numRanges = _countRanges(input);
        auto rangeList = std::vector<std::string>();
        rangeList.reserve(numRanges);

        size_t start = 0;
        for (size_t i = 1; i < input.size(); ++i)
        {
            if (input[i] == input[i - 1] + 1)
            {
                continue;
            }

            auto rangeStart = input[start];
            auto rangeEnd = input[i - 1];
            rangeList.push_back(_buildRange(rangeStart, rangeEnd));

            start = i;
        }

        return rangeList;
    }
};

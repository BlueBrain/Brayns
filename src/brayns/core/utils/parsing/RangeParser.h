/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/utils/string/StringCounter.h>
#include <brayns/core/utils/string/StringExtractor.h>
#include <brayns/core/utils/string/StringInfo.h>
#include <brayns/core/utils/string/StringParser.h>

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace brayns
{
template<typename T>
class RangeParser
{
public:
    static_assert(std::is_integral_v<T>, "Only integral ranges allowed");

    static std::vector<T> parse(std::string_view data)
    {
        auto rangeList = _splitRangeList(data);
        auto ranges = _parseRangeList(rangeList);
        auto list = _flattenRangeList(ranges);
        std::sort(list.begin(), list.end());
        return list;
    }

private:
    struct Range
    {
        T start;
        T end;
    };

    static std::vector<std::string_view> _splitRangeList(std::string_view input)
    {
        if (!StringInfo::contains(input, ","))
        {
            return {input};
        }

        auto rangeCount = StringCounter::count(input, ",") + 1;

        auto ranges = std::vector<std::string_view>();
        ranges.reserve(rangeCount);

        size_t i = 0;
        while (i++ < rangeCount)
        {
            auto range = StringExtractor::extractUntil(input, ",");
            StringExtractor::extract(input, 1);
            ranges.push_back(range);
        }

        return ranges;
    }

    static Range _parseRange(std::string_view range)
    {
        if (!StringInfo::contains(range, "-"))
        {
            T start;
            StringParser<T>::parse(range, start);
            return {start, start};
        }

        auto startToken = StringExtractor::extractUntil(range, "-");
        StringExtractor::extract(range, 1);
        T start, end;
        StringParser<T>::parse(startToken, start);
        StringParser<T>::parse(range, end);

        if (start > end)
        {
            throw std::range_error(fmt::format("Invalid range {}-{}", start, end));
        }

        return {start, end};
    }

    static std::vector<Range> _parseRangeList(const std::vector<std::string_view> &ranges)
    {
        auto rangeList = std::vector<Range>();
        rangeList.reserve(ranges.size());

        for (auto range : ranges)
        {
            rangeList.push_back(_parseRange(range));
        }

        return rangeList;
    }

    static size_t _countElementsFromRanges(const std::vector<Range> &ranges)
    {
        size_t elementCount = 0;
        for (auto &range : ranges)
        {
            elementCount += range.end + 1 - range.start;
        }
        return elementCount;
    }

    static std::vector<T> _flattenRangeList(const std::vector<Range> &ranges)
    {
        auto elementCount = _countElementsFromRanges(ranges);

        auto result = std::vector<T>();
        result.reserve(elementCount);

        for (auto &range : ranges)
        {
            for (T i = range.start; i < range.end + 1; ++i)
            {
                result.push_back(i);
            }
        }

        return result;
    }
};
}

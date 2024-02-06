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

#include "StringSplitter.h"

#include "StringCounter.h"
#include "StringExtractor.h"

namespace brayns
{
std::vector<std::string> StringSplitter::split(std::string_view data, char separator)
{
    std::vector<std::string> result;
    auto count = StringCounter::count(data, separator);
    result.reserve(count + 1);
    while (true)
    {
        auto slice = StringExtractor::extractUntil(data, separator);
        result.emplace_back(slice);
        if (data.empty())
        {
            return result;
        }
        StringExtractor::extract(data, 1);
    }
}

std::vector<std::string> StringSplitter::split(std::string_view data, std::string_view separator)
{
    std::vector<std::string> result;
    auto count = StringCounter::count(data, separator);
    result.reserve(count + 1);
    while (true)
    {
        auto slice = StringExtractor::extractUntil(data, separator);
        result.emplace_back(slice);
        if (data.empty())
        {
            return result;
        }
        auto size = separator.size();
        StringExtractor::extract(data, size);
    }
}

std::vector<std::string> StringSplitter::splitOneOf(std::string_view data, std::string_view separators)
{
    std::vector<std::string> result;
    auto count = StringCounter::countOneOf(data, separators);
    result.reserve(count + 1);
    while (true)
    {
        auto slice = StringExtractor::extractUntilOneOf(data, separators);
        result.emplace_back(slice);
        if (data.empty())
        {
            return result;
        }
        StringExtractor::extract(data, 1);
    }
}

std::vector<std::string> StringSplitter::splitTokens(std::string_view data)
{
    std::vector<std::string> result;
    auto count = StringCounter::countTokens(data);
    result.reserve(count);
    while (true)
    {
        auto slice = StringExtractor::extractToken(data);
        if (slice.empty())
        {
            return result;
        }
        result.emplace_back(slice);
    }
}

std::vector<std::string> StringSplitter::splitLines(std::string_view data)
{
    return split(data, '\n');
}
} // namespace brayns

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

#include "StringExtractor.h"

#include <algorithm>
#include <utility>

#include "StringInfo.h"
#include "StringTrimmer.h"

namespace brayns
{
bool StringExtractor::canExtract(std::string_view data, size_t size)
{
    return data.size() >= size;
}

std::string_view StringExtractor::extractAll(std::string_view &data)
{
    return std::exchange(data, {});
}

std::string_view StringExtractor::extract(std::string_view &data, size_t size)
{
    if (!canExtract(data, size))
    {
        return extractAll(data);
    }
    auto result = data.substr(0, size);
    data.remove_prefix(size);
    return result;
}

std::string_view StringExtractor::extractUntil(std::string_view &data, char separator)
{
    auto index = data.find(separator);
    return extract(data, index);
}

std::string_view StringExtractor::extractUntil(std::string_view &data, std::string_view separator)
{
    auto index = data.find(separator);
    return extract(data, index);
}

std::string_view StringExtractor::extractUntilOneOf(std::string_view &data, std::string_view separators)
{
    auto index = data.find_first_of(separators);
    return extract(data, index);
}

std::string_view StringExtractor::extractToken(std::string_view &data)
{
    extractSpaces(data);
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (!StringInfo::isSpace(data[i]))
        {
            continue;
        }
        return extract(data, i);
    }
    return extractAll(data);
}

std::string_view StringExtractor::extractLine(std::string_view &data)
{
    return extractUntil(data, '\n');
}

void StringExtractor::extractSpaces(std::string_view &data)
{
    data = StringTrimmer::trimLeft(data);
}
} // namespace brayns

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

#include "StringInfo.h"

#include <algorithm>
#include <cctype>

#include "StringExtractor.h"

namespace brayns
{
bool StringInfo::isSpace(char data)
{
    return std::isspace(static_cast<unsigned char>(data));
}

bool StringInfo::isSpace(std::string_view data)
{
    return std::all_of(data.begin(), data.end(), [](auto c) { return isSpace(c); });
}

bool StringInfo::isLower(char data)
{
    return std::islower(static_cast<unsigned char>(data));
}

bool StringInfo::isLower(std::string_view data)
{
    return std::all_of(data.begin(), data.end(), [](auto c) { return isLower(c); });
}

bool StringInfo::isUpper(char data)
{
    return std::isupper(static_cast<unsigned char>(data));
}

bool StringInfo::isUpper(std::string_view data)
{
    return std::all_of(data.begin(), data.end(), [](auto c) { return isUpper(c); });
}

bool StringInfo::startsWith(std::string_view data, char prefix)
{
    return !data.empty() && data[0] == prefix;
}

bool StringInfo::startsWith(std::string_view data, std::string_view prefix)
{
    auto step = prefix.size();
    return !data.empty() && data.substr(0, step) == prefix;
}

bool StringInfo::endsWith(std::string_view data, char suffix)
{
    return !data.empty() && data.back() == suffix;
}

bool StringInfo::endsWith(std::string_view data, std::string_view suffix)
{
    auto size = data.size();
    auto step = suffix.size();
    return size >= step && data.substr(size - step) == suffix;
}

bool StringInfo::contains(std::string_view data, char item)
{
    return data.find(item) != std::string_view::npos;
}

bool StringInfo::contains(std::string_view data, std::string_view item)
{
    return data.find(item) != std::string_view::npos;
}

bool StringInfo::containsOneOf(std::string_view data, std::string_view items)
{
    return data.find_first_of(items) != std::string_view::npos;
}

bool StringInfo::containsToken(std::string_view data, std::string_view token)
{
    while (!isSpace(data))
    {
        auto slice = StringExtractor::extractToken(data);
        if (slice == token)
        {
            return true;
        }
    }
    return false;
}
} // namespace brayns

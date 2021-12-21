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

#include "StringHelper.h"

#include <algorithm>

namespace brayns
{
bool StringHelper::isSpace(char c)
{
    return c == ' ' || c == '\t';
}

bool StringHelper::isSpace(std::string_view str)
{
    return std::all_of(str.begin(), str.end(),
                       [](auto c) { return isSpace(c); });
}

bool StringHelper::startsWith(std::string_view str, std::string_view item)
{
    return str.substr(0, item.size()) == item;
}

std::string_view StringHelper::trimLeft(std::string_view str)
{
    for (size_t i = 0; i < str.size(); ++i)
    {
        auto c = str[i];
        if (!isSpace(c))
        {
            return str.substr(i);
        }
    }
    return {};
}

size_t StringHelper::count(std::string_view str, std::string_view item)
{
    size_t result = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        auto extracted = str.substr(i, item.size());
        if (extracted == item)
        {
            ++result;
        }
    }
    return result;
}

std::string_view StringHelper::extract(std::string_view &str,
                                       std::string_view separator)
{
    auto size = separator.size();
    size_t i = 0;
    while (i < str.size() && str.substr(i, size) != separator)
    {
        ++i;
    }
    auto extracted = str.substr(0, i);
    str = str.substr(i + size);
    return extracted;
}

std::string_view StringHelper::extractToken(std::string_view &str)
{
    str = trimLeft(str);
    size_t i = 0;
    while (i < str.size() && isSpace(str[i]))
    {
        ++i;
    }
    auto token = str.substr(0, i);
    str = trimLeft(str.substr(i));
    return token;
}

size_t StringHelper::countTokens(std::string_view str)
{
    size_t result;
    while (!extractToken(str).empty())
    {
        ++result;
    }
    return result;
}
} // namespace brayns
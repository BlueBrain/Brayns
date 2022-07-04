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

#include "StringCounter.h"

#include "StringInfo.h"

namespace brayns
{
size_t StringCounter::count(std::string_view data, char item)
{
    return count(data, {&item, 1});
}

size_t StringCounter::count(std::string_view data, std::string_view item)
{
    size_t result = 0;
    while (true)
    {
        auto index = data.find(item);
        if (index == std::string_view::npos)
        {
            return result;
        }
        ++result;
        auto start = index + item.size();
        data = data.substr(start);
    }
}

size_t StringCounter::countOneOf(std::string_view data, std::string_view items)
{
    size_t result = 0;
    while (true)
    {
        auto index = data.find_first_of(items);
        if (index == std::string_view::npos)
        {
            return result;
        }
        ++result;
        data = data.substr(index + 1);
    }
    return result;
}

size_t StringCounter::countTokens(std::string_view data)
{
    size_t result = 0;
    bool token = false;
    for (auto c : data)
    {
        if (StringInfo::isSpace(c))
        {
            token = false;
            continue;
        }
        if (token)
        {
            continue;
        }
        token = true;
        ++result;
    }
    return result;
}

size_t StringCounter::countLines(std::string_view data)
{
    return count(data, '\n') + 1;
}
} // namespace brayns

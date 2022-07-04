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

#include "StringStream.h"

#include <algorithm>

#include "StringInfo.h"
#include "StringTrimmer.h"

namespace brayns
{
StringStream::StringStream(std::string_view data)
    : _data(data)
{
}

bool StringStream::isEmpty() const
{
    return _data.empty();
}

std::string_view StringStream::extract(char separator)
{
    return extract({&separator, 1});
}

std::string_view StringStream::extract(std::string_view separator)
{
    auto index = _data.find(separator);
    if (index == std::string_view::npos)
    {
        return std::exchange(_data, {});
    }
    auto result = _data.substr(0, index);
    auto start = index + separator.size();
    _data = _data.substr(start);
    return result;
}

std::string_view StringStream::extractOneOf(std::string_view separators)
{
    auto index = _data.find_first_of(separators);
    if (index == std::string_view::npos)
    {
        return std::exchange(_data, {});
    }
    auto result = _data.substr(0, index);
    _data = _data.substr(index + 1);
    return result;
}

std::string_view StringStream::extractToken()
{
    _data = StringTrimmer::trimLeft(_data);
    for (size_t i = 0; i < _data.size(); ++i)
    {
        if (!StringInfo::isSpace(_data[i]))
        {
            continue;
        }
        auto result = _data.substr(0, i);
        _data = _data.substr(i);
        return result;
    }
    return std::exchange(_data, {});
}

std::string_view StringStream::extractLine()
{
    return extract('\n');
}
} // namespace brayns

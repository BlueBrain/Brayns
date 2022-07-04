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

#pragma once

#include <string_view>

#include "StringParser.h"

namespace brayns
{
class StringStream
{
public:
    StringStream() = default;
    StringStream(std::string_view data);

    bool isEmpty() const;
    std::string_view extractAll();
    std::string_view extract(size_t count);
    std::string_view extractUntil(char separator);
    std::string_view extractUntil(std::string_view separator);
    std::string_view extractUntilOneOf(std::string_view separators);
    std::string_view extractToken();
    std::string_view extractLine();

    template<typename T>
    T extractAll()
    {
        auto data = extractAll();
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extract(size_t count)
    {
        auto data = extract(count);
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extractUntil(char separator)
    {
        auto data = extractUntil(separator);
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extractUntil(std::string_view separator)
    {
        auto data = extractUntil(separator);
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extractUntilOneOf(std::string_view separators)
    {
        auto data = extractUntilOneOf(separators);
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extractToken()
    {
        auto data = extractToken();
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extractLine()
    {
        auto data = extractLine();
        return StringParser<T>::parse(data);
    }

private:
    std::string_view _data;
};
} // namespace brayns

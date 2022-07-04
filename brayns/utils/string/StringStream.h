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
    std::string_view extract(char separator);
    std::string_view extract(std::string_view separator);
    std::string_view extractOneOf(std::string_view separators);
    std::string_view extractToken();
    std::string_view extractLine();

    template<typename T>
    T extract(char separator)
    {
        auto data = extract(separator);
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extract(std::string_view separator)
    {
        auto data = extract(separator);
        return StringParser<T>::parse(data);
    }

    template<typename T>
    T extractOneOf(std::string_view separators)
    {
        auto data = extractOneOf(separators);
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

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

#include "StringConverter.h"

#include <cctype>

namespace brayns
{
char StringConverter::toLower(char c)
{
    return std::tolower(static_cast<unsigned char>(c));
}

char StringConverter::toUpper(char c)
{
    return std::toupper(static_cast<unsigned char>(c));
}

std::string StringConverter::toLower(std::string_view data)
{
    auto result = std::string(data);
    lower(result);
    return result;
}

std::string StringConverter::toUpper(std::string_view data)
{
    auto result = std::string(data);
    upper(result);
    return result;
}

void StringConverter::lower(std::string &data)
{
    for (auto &c : data)
    {
        c = toLower(c);
    }
}

void StringConverter::upper(std::string &data)
{
    for (auto &c : data)
    {
        c = toUpper(c);
    }
}
} // namespace brayns

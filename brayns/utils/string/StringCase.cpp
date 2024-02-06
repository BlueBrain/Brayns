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

#include "StringCase.h"

#include <cctype>

namespace brayns
{
char StringCase::toLower(char c)
{
    auto lower = std::tolower(static_cast<unsigned char>(c));
    return static_cast<char>(lower);
}

char StringCase::toUpper(char c)
{
    auto upper = std::toupper(static_cast<unsigned char>(c));
    return static_cast<char>(upper);
}

std::string StringCase::toLower(std::string_view data)
{
    auto result = std::string(data);
    lower(result);
    return result;
}

std::string StringCase::toUpper(std::string_view data)
{
    auto result = std::string(data);
    upper(result);
    return result;
}

void StringCase::lower(std::string &data)
{
    for (auto &c : data)
    {
        c = toLower(c);
    }
}

void StringCase::upper(std::string &data)
{
    for (auto &c : data)
    {
        c = toUpper(c);
    }
}
} // namespace brayns

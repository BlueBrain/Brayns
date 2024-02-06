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

#pragma once

#include <string_view>

namespace brayns
{
class StringInfo
{
public:
    static bool isSpace(char data);
    static bool isSpace(std::string_view data);
    static bool isLower(char data);
    static bool isLower(std::string_view data);
    static bool isUpper(char data);
    static bool isUpper(std::string_view data);
    static bool startsWith(std::string_view data, char prefix);
    static bool startsWith(std::string_view data, std::string_view prefix);
    static bool endsWith(std::string_view data, char suffix);
    static bool endsWith(std::string_view data, std::string_view suffix);
    static bool contains(std::string_view data, char item);
    static bool contains(std::string_view data, std::string_view item);
    static bool containsOneOf(std::string_view data, std::string_view items);
    static bool containsToken(std::string_view data, std::string_view token);
};
} // namespace brayns

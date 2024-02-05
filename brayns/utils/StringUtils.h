/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <string>
#include <vector>

namespace brayns
{
namespace string_utils
{
std::string shortenString(const std::string& string,
                          const size_t maxLength = 32);

std::string replaceFirstOccurrence(std::string input,
                                   const std::string& toReplace,
                                   const std::string& replaceWith);

std::string camelCaseToSeparated(const std::string& camelCase,
                                 const char separator,
                                 bool separateNumericals = true);

std::string separatedToCamelCase(const std::string& separated,
                                 const char separator);

std::string join(const std::vector<std::string>& strings,
                 const std::string& joinWith);

std::string toLowercase(const std::string input);

void trim(std::string& s);

std::vector<std::string> split(const std::string& s, char delim);
} // namespace string_utils
} // namespace brayns

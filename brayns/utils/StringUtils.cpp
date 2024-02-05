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

#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <locale>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace brayns
{
namespace string_utils
{
const std::string ELLIPSIS("...");

std::string shortenString(const std::string& string, const size_t maxLength)
{
    if (string.length() <= maxLength)
        return string;

    const float spacePerPart = (maxLength - ELLIPSIS.length()) / 2.f;
    const auto beforeEllipsis = string.substr(0, std::ceil(spacePerPart));
    const auto afterEllipsis =
        string.substr(string.size() - std::floor(spacePerPart));

    return beforeEllipsis + ELLIPSIS + afterEllipsis;
}

std::string replaceFirstOccurrence(std::string input,
                                   const std::string& toReplace,
                                   const std::string& replaceWith)
{
    std::size_t pos = input.find(toReplace);
    if (pos == std::string::npos)
        return input;
    return input.replace(pos, toReplace.length(), replaceWith);
}

std::string camelCaseToSeparated(const std::string& camelCase,
                                 const char separator, bool separateNumericals)
{
    if (camelCase.empty())
        return camelCase;

    std::string str(1, ::tolower(camelCase[0]));
    for (auto it = camelCase.begin() + 1; it != camelCase.end(); ++it)
    {
        if ((isupper(*it) && *(it - 1) != '-' && islower(*(it - 1))) ||
            (separateNumericals && isdigit(*(it - 1)) && isalpha(*it)))
            str += separator;
        str += *it;
    }

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string separatedToCamelCase(const std::string& separated,
                                 const char separator)
{
    std::string camel = separated;

    for (size_t x = 0; x < camel.length(); x++)
    {
        if (camel[x] == separator)
        {
            std::string tempString = camel.substr(x + 1, 1);

            std::transform(tempString.begin(), tempString.end(),
                           tempString.begin(), ::toupper);

            camel.erase(x, 2);
            camel.insert(x, tempString);
        }
    }
    return camel;
}

std::string join(const std::vector<std::string>& strings,
                 const std::string& joinWith)
{
    const size_t numStrings = strings.size();
    if (numStrings == 0)
        return "";

    std::stringstream ss;
    ss << strings[0];
    for (size_t i = 1; i < numStrings; i++)
        ss << joinWith << strings[i];
    return ss.str();
}

std::string toLowercase(std::string input)
{
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));
}

void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim))
        elems.push_back(std::move(item));
    return elems;
}
} // namespace string_utils
} // namespace brayns

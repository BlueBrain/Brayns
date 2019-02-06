/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "utils.h"

#include <brayns/common/log.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <algorithm>
#include <set>
#include <sstream>
#include <string>

namespace brayns
{
strings parseFolder(const std::string& folder, const strings& filters)
{
    strings files;
    fs::directory_iterator endIter;
    if (fs::is_directory(folder))
    {
        for (fs::directory_iterator dirIter(folder); dirIter != endIter;
             ++dirIter)
        {
            if (fs::is_regular_file(dirIter->status()))
            {
                const auto filename = dirIter->path().c_str();
                if (filters.empty())
                    files.push_back(filename);
                else
                {
                    const auto& fileExtension = dirIter->path().extension();
                    const auto found = std::find(filters.begin(), filters.end(),
                                                 fileExtension);
                    if (found != filters.end())
                        files.push_back(filename);
                }
            }
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

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

std::string extractExtension(const std::string& filename)
{
    auto extension = fs::extension(filename);
    if (!extension.empty())
        extension = extension.erase(0, 1);

    return extension;
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
                                 const char separator)
{
    if (camelCase.empty())
        return camelCase;

    std::string str(1, tolower(camelCase[0]));
    for (auto it = camelCase.begin() + 1; it != camelCase.end(); ++it)
    {
        if (isupper(*it) && *(it - 1) != '-' && islower(*(it - 1)))
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

            transform(tempString.begin(), tempString.end(), tempString.begin(),
                      toupper);

            camel.erase(x, 2);
            camel.insert(x, tempString);
        }
    }
    return camel;
}

bool containsString(const int length, const char** input, const char* toFind)
{
    return std::count_if(input, input + length, [toFind](const char* arg) {
               return std::strcmp(arg, toFind) == 0;
           }) > 0;
}

std::string joinStrings(const std::vector<std::string>& strings,
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

} // namespace brayns

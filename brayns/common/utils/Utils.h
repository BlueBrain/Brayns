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

#ifndef UTILS_H
#define UTILS_H

#include <brayns/common/types.h>

namespace brayns
{
strings parseFolder(const std::string& folder, const strings& filters);

const std::string ELLIPSIS("...");
inline std::string shortenString(const std::string& string,
                                 const size_t maxLength = 32)
{
    if (string.length() <= maxLength)
        return string;

    const float spacePerPart = (maxLength - ELLIPSIS.length()) / 2.f;
    const auto beforeEllipsis = string.substr(0, std::ceil(spacePerPart));
    const auto afterEllipsis =
        string.substr(string.size() - std::floor(spacePerPart));

    return beforeEllipsis + ELLIPSIS + afterEllipsis;
}

inline auto lowerCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

/**
 * @brief Determines the name of the file according to its full path
 * @param fileName Full path to of the file
 * @return Name of the file
 */
std::string getNameFromFullPath(const std::string& fileName);

}

#endif // UTILS_H

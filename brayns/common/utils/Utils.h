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

std::string shortenString(const std::string& string,
                          const size_t maxLength = 32);

inline auto lowerCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

bool isSupportedArchiveType(const std::string& extension);
bool isArchive(const std::string& filename);
bool isArchive(const Blob& blob);
void extractFile(const std::string& filename, const std::string& destination);
void extractBlob(Blob&& blob, const std::string& destination);
}

#endif // UTILS_H

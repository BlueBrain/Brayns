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
#include <brayns/common/utils/filesystem.h>

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

std::string extractExtension(const std::string& filename)
{
    auto extension = fs::path(filename).extension().string();
    if (!extension.empty())
        extension = extension.erase(0, 1);

    return extension;
}

} // namespace brayns

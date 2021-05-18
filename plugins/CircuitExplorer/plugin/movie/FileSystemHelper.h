/* Copyright (c) 2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

#include <algorithm>
#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

class FileSystemHelper
{
public:
    using DirectoryIterator = boost::filesystem::directory_iterator;
    using Path = boost::filesystem::path;

    static bool isDirectory(const std::string& path)
    {
        return boost::filesystem::is_directory(path);
    }

    static void removeFiles(const std::vector<std::string>& entries)
    {
        for (const auto& entry : entries)
        {
            boost::filesystem::remove(entry);
        }
    }

    static void removeDirectory(const std::string& path)
    {
        boost::filesystem::remove_all(path);
    }

    static std::string getExtensionWithDot(const std::string& extension)
    {
        if (extension.empty() || extension[0] == '.')
        {
            return extension;
        }
        return "." + extension;
    }

    static std::vector<std::string> getDirectoryFiles(
        const std::string& path, const std::string& extension)
    {
        auto extensionWithDot = getExtensionWithDot(extension);
        std::vector<std::string> entries;
        for (const auto& entry : DirectoryIterator(path))
        {
            if (!boost::filesystem::is_regular_file(entry))
            {
                continue;
            }
            auto& path = entry.path();
            if (path.extension() != extensionWithDot)
            {
                continue;
            }
            entries.push_back(path.native());
        }
        return entries;
    }

    static std::vector<std::string> getSortedDirectoryFiles(
        const std::string& path, const std::string& extension)
    {
        auto entries = getDirectoryFiles(path, extension);
        std::sort(entries.begin(), entries.end());
        return entries;
    }
};
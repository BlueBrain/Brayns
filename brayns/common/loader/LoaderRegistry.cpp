/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur <daniel.nachbaur@epfl.ch>
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

#include "LoaderRegistry.h"

#include <brayns/common/utils/Utils.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace brayns
{
void LoaderRegistry::registerLoader(LoaderInfo loaderInfo)
{
    _loaders.push_back(loaderInfo);
}

bool LoaderRegistry::isSupported(const std::string& type) const
{
    for (const auto& entry : _loaders)
    {
        if (_isSupported(entry, type))
            return true;
    }
    return false;
}

std::set<std::string> LoaderRegistry::supportedTypes() const
{
    std::set<std::string> result;
    for (const auto& entry : _loaders)
    {
        const auto& types = entry.supportedTypes();
        result.insert(types.begin(), types.end());
    }
    return result;
}

LoaderPtr LoaderRegistry::createLoader(const std::string& type) const
{
    for (const auto& entry : _loaders)
    {
        if (!_isSupported(entry, type))
            continue;
        return entry.createLoader();
    }
    throw std::runtime_error("No loader found for " + type);
}

bool LoaderRegistry::_isSupported(const LoaderInfo& loader,
                                  const std::string& type) const
{
    // the first file in the folder that is supported by this loader wins
    if (fs::is_directory(type))
    {
        for (const auto& i :
             boost::make_iterator_range(fs::directory_iterator(type), {}))
        {
            if (_isSupported(loader, i.path().string()))
                return true;
        }
        return false;
    }
    auto extension = fs::extension(type);
    if (extension.empty())
    {
        // if the path has no extension, treat the filename as the type
        if (fs::is_regular_file(type))
            extension = fs::path(type).filename().string();
        else
            extension = type; // just the type from blob, e.g. xyz, obj, ...
    }
    else
        extension = extension.erase(0, 1);

    if (isSupportedArchiveType(extension))
        return true;

    const auto& types = loader.supportedTypes();
    const auto found =
        std::find_if(types.cbegin(), types.cend(), [extension](auto val) {
            return lowerCase(val) == lowerCase(extension);
        });
    return found != types.end();
}
}

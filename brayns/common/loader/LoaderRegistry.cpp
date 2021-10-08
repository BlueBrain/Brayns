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

#include <brayns/common/utils/filesystem.h>
#include <brayns/common/utils/utils.h>

namespace brayns
{
void LoaderRegistry::registerLoader(std::unique_ptr<Loader> loader)
{
    _loaderInfos.push_back({loader->getName(), loader->getSupportedExtensions(),
                            loader->getProperties()});
    _loaders.push_back(std::move(loader));
}

const std::vector<LoaderInfo>& LoaderRegistry::getLoaderInfos() const
{
    return _loaderInfos;
}

bool LoaderRegistry::isSupportedFile(const std::string& filename) const
{
    if (fs::is_directory(filename))
        return false;

    const auto extension = extractExtension(filename);
    if (_archiveSupported(filename, extension))
        return true;
    for (const auto& loader : _loaders)
        if (loader->isSupported(filename, extension))
            return true;
    return false;
}

bool LoaderRegistry::isSupportedType(const std::string& type) const
{
    if (_archiveSupported("", type))
        return true;
    for (const auto& loader : _loaders)
        if (loader->isSupported("", type))
            return true;
    return false;
}

const Loader& LoaderRegistry::getSuitableLoader(
    const std::string& filename, const std::string& filetype,
    const std::string& loaderName) const
{
    if (fs::is_directory(filename))
        throw std::runtime_error("'" + filename + "' is a directory");

    const auto extension =
        filetype.empty() ? extractExtension(filename) : filetype;

    // If we have an archive we always use the archive loader even if a specific
    // loader is specified
    if (_archiveSupported(filename, extension))
        return *_archiveLoader;

    // Find specific loader
    if (!loaderName.empty())
    {
        for (const auto& loader : _loaders)
            if (loader->getName() == loaderName)
                return *loader.get();

        throw std::runtime_error("No loader found with name '" + loaderName +
                                 "'");
    }

    for (const auto& loader : _loaders)
        if (loader->isSupported(filename, extension))
            return *loader;

    throw std::runtime_error("No loader found for filename '" + filename +
                             "' and filetype '" + filetype + "'");
}

void LoaderRegistry::clear()
{
    _loaders.clear();
    _archiveLoader.reset();
    _loaderInfos.clear();
}

void LoaderRegistry::registerArchiveLoader(std::unique_ptr<Loader> loader)
{
    _archiveLoader = std::move(loader);
}

bool LoaderRegistry::_archiveSupported(const std::string& filename,
                                       const std::string& filetype) const
{
    return _archiveLoader && _archiveLoader->isSupported(filename, filetype);
}
} // namespace brayns

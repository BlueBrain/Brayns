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
#include <boost/range/iterator_range_core.hpp>
namespace fs = boost::filesystem;

namespace
{
std::string extract_extension(const std::string& filename)
{
    auto extension = fs::extension(filename);
    if (!extension.empty())
        extension = extension.erase(0, 1);

    return extension;
}
}

namespace brayns
{
void LoaderRegistry::registerLoader(std::unique_ptr<Loader> loader)
{
    _loaders.push_back(std::move(loader));
}

bool LoaderRegistry::isSupportedFile(const std::string& filename) const
{
    if (fs::is_directory(filename))
        return false;

    const auto extension = extract_extension(filename);
    for (const auto& loader : _loaders)
        if (loader->isSupported(filename, extension))
            return true;
    return false;
}

bool LoaderRegistry::isSupportedType(const std::string& type) const
{
    for (const auto& loader : _loaders)
        if (loader->isSupported("", type))
            return true;
    return false;
}

const Loader& LoaderRegistry::getLoaderFromFilename(
    const std::string& filename) const
{
    if (fs::is_directory(filename))
        throw std::runtime_error("'" + filename + "' is a directory");

    const auto extension = extract_extension(filename);

    for (const auto& loader : _loaders)
        if (loader->isSupported(filename, extension))
            return *loader;

    throw std::runtime_error("No loader found for filename '" + filename + "'");
}

const Loader& LoaderRegistry::getLoaderFromFiletype(
    const std::string& filetype) const
{
    for (const auto& loader : _loaders)
        if (loader->isSupported("", filetype))
            return *loader;

    throw std::runtime_error("No loader found for type '" + filetype + "'");
}
}

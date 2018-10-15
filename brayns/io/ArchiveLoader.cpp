/* Copyright (c) 2018 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include "ArchiveLoader.h"

#include <brayns/common/log.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/range.hpp>
namespace fs = boost::filesystem;

namespace brayns
{
ArchiveLoader::ArchiveLoader(Scene& scene, LoaderRegistry& registry)
    : Loader(scene)
    , _registry(registry)
{
}

bool ArchiveLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                                const std::string& extension) const
{
    return isSupportedArchiveType(extension);
}

ModelDescriptorPtr ArchiveLoader::loadExtracted(
    const std::string& path, const LoaderProgress& callback, const size_t index,
    const size_t defaultMaterialId) const
{
    for (const auto& i :
         boost::make_iterator_range(fs::directory_iterator(path), {}))
    {
        const std::string currPath = i.path().string();
        if (_registry.isSupportedFile(currPath))
        {
            const auto& loader = _registry.getLoaderFromFilename(currPath);
            return loader.importFromFile(currPath, callback, index,
                                         defaultMaterialId);
        }
    }
    throw std::runtime_error("No loader found for archive.");
}

ModelDescriptorPtr ArchiveLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    fs::path path = fs::temp_directory_path() / fs::unique_path();
    extractBlob(std::move(blob), path.string());
    return loadExtracted(path.string(), callback, index, defaultMaterialId);
}

ModelDescriptorPtr ArchiveLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const size_t index, const size_t defaultMaterialId) const
{
    fs::path path = fs::temp_directory_path() / fs::unique_path();
    extractFile(filename, path.string());
    return loadExtracted(path.string(), callback, index, defaultMaterialId);
}
}

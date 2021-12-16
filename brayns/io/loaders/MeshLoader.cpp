/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "MeshLoader.h"

#include "mesh/parsers/ObjMeshParser.h"

namespace brayns
{
MeshLoader::MeshLoader()
{
    add<ObjMeshParser>();
}

std::vector<std::string> MeshLoader::getSupportedExtensions() const
{
    std::vector<std::string> extensions;
    extensions.reserve(_parsers.size());
    for (const auto& pair : _parsers)
    {
        auto& format = pair.first;
        extensions.push_back(format);
    }
    return extensions;
}

std::string MeshLoader::getName() const
{
    return "mesh";
}

std::vector<ModelDescriptorPtr> MeshLoader::importFromFile(
    const std::string& fileName, const LoaderProgress& callback,
    const MeshLoaderParameters& params, Scene& scene) const
{
    return {};
}

std::vector<ModelDescriptorPtr> MeshLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const MeshLoaderParameters& params, Scene& scene) const
{
    return {};
}
} // namespace brayns

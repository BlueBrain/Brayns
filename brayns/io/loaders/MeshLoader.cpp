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

#include <filesystem>
#include <fstream>
#include <sstream>

#include <brayns/common/DefaultMaterialIds.h>
#include <brayns/engine/Scene.h>

#include "mesh/parsers/ObjMeshParser.h"

namespace
{
using namespace brayns;

class MeshParsingHelper
{
public:
    static std::vector<TriangleMesh> parse(const MeshParserRegistry& parsers,
                                           const std::string& filename)
    {
        std::filesystem::path path(filename);
        auto extension = path.extension();
        auto format = extension.string();
        format.erase(0, 1);
        std::ifstream stream(filename);
        if (!stream)
        {
            throw std::runtime_error("Cannot open '" + filename + "'");
        }
        return parse(parsers, format, stream);
    }

    static std::vector<TriangleMesh> parse(const MeshParserRegistry& parsers,
                                           const Blob& blob)
    {
        auto& format = blob.type;
        auto& data = blob.data;
        auto ptr = reinterpret_cast<const char*>(data.data());
        auto size = data.size();
        std::string buffer = {ptr, size};
        std::istringstream stream(buffer);
        return parse(parsers, format, stream);
    }

    static std::vector<TriangleMesh> parse(const MeshParserRegistry& parsers,
                                           const std::string& format,
                                           std::istream& stream)
    {
        auto& parser = parsers.getParser(format);
        auto meshes = parser.parse(stream);
        if (meshes.empty())
        {
            throw std::runtime_error("No meshes found");
        }
        return meshes;
    }
};

class MeshLoadingHelper
{
public:
    static ModelPtr load(const std::vector<TriangleMesh>& meshes, Scene& scene)
    {
        auto model = scene.createModel();
        model->createMaterial(NO_MATERIAL, "default");
        auto& modelMeshes = model->getTriangleMeshes();
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            modelMeshes[i] = meshes[i];
        }
        return model;
    }
};

class MeshMetadata
{
public:
    static ModelMetadata build(const std::vector<TriangleMesh>& meshes)
    {
        auto meshCount = meshes.size();
        size_t vertexCount = 0;
        size_t triangleCount = 0;
        for (const auto& mesh : meshes)
        {
            vertexCount += mesh.vertices.size();
            triangleCount += mesh.indices.size();
        }
        return {{"meshes", std::to_string(meshCount)},
                {"vertices", std::to_string(vertexCount)},
                {"faces", std::to_string(triangleCount)}};
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> MeshParserRegistry::getAllFormats() const
{
    std::vector<std::string> formats;
    formats.reserve(_parsers.size());
    for (const auto& [format, parser] : _parsers)
    {
        formats.push_back(format);
    }
    return formats;
}

const MeshParser& MeshParserRegistry::getParser(const std::string& format) const
{
    auto parser = findParser(format);
    if (!parser)
    {
        throw std::runtime_error("Mesh format not supported: '" + format + "'");
    }
    return *parser;
}

const MeshParser* MeshParserRegistry::findParser(
    const std::string& format) const
{
    auto i = _parsers.find(format);
    return i == _parsers.end() ? nullptr : i->second.get();
}

void MeshParserRegistry::addParser(std::unique_ptr<MeshParser> parser)
{
    auto format = parser->getFormat();
    auto [i, ok] = _parsers.emplace(std::move(format), std::move(parser));
    if (!ok)
    {
        throw std::runtime_error("Mesh format duplication: '" + format + "'");
    }
}

MeshLoader::MeshLoader()
{
    _parsers.add<ObjMeshParser>();
}

std::vector<std::string> MeshLoader::getSupportedExtensions() const
{
    return _parsers.getAllFormats();
}

std::string MeshLoader::getName() const
{
    return "mesh";
}

std::vector<ModelDescriptorPtr> MeshLoader::importFromFile(
    const std::string& fileName, const LoaderProgress& callback,
    const MeshLoaderParameters& params, Scene& scene) const
{
    (void)callback;
    (void)params;
    auto meshes = MeshParsingHelper::parse(_parsers, fileName);
    auto model = MeshLoadingHelper::load(meshes, scene);
    auto metadata = MeshMetadata::build(meshes);
    auto& name = fileName;
    auto descriptor =
        std::make_shared<ModelDescriptor>(std::move(model), name, metadata);
    return {descriptor};
}

std::vector<ModelDescriptorPtr> MeshLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const MeshLoaderParameters& params, Scene& scene) const
{
    (void)callback;
    (void)params;
    auto meshes = MeshParsingHelper::parse(_parsers, blob);
    auto model = MeshLoadingHelper::load(meshes, scene);
    auto metadata = MeshMetadata::build(meshes);
    auto& name = blob.name;
    auto descriptor =
        std::make_shared<ModelDescriptor>(std::move(model), name, metadata);
    return {descriptor};
}
} // namespace brayns

/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
#include <brayns/utils/FileReader.h>

#include "mesh/parsers/ObjMeshParser.h"
#include "mesh/parsers/OffMeshParser.h"
#include "mesh/parsers/PlyMeshParser.h"
#include "mesh/parsers/StlMeshParser.h"

namespace
{
class MeshParsingHelper
{
public:
    static brayns::TriangleMesh parse(const brayns::MeshParserRegistry &parsers, const std::string &filename)
    {
        std::filesystem::path path(filename);
        auto extension = path.extension();
        auto format = extension.string();
        format.erase(0, 1);
        auto data = brayns::FileReader::read(filename);
        return parse(parsers, format, data);
    }

    static brayns::TriangleMesh parse(const brayns::MeshParserRegistry &parsers, const brayns::Blob &blob)
    {
        auto &format = blob.type;
        auto &blobData = blob.data;
        auto ptr = reinterpret_cast<const char *>(blobData.data());
        auto size = blobData.size();
        std::string_view data = {ptr, size};
        return parse(parsers, format, data);
    }

    static brayns::TriangleMesh
        parse(const brayns::MeshParserRegistry &parsers, const std::string &format, std::string_view data)
    {
        auto &parser = parsers.getParser(format);
        auto mesh = parser.parse(data);
        if (mesh.indices.empty())
        {
            throw std::runtime_error("No meshes found");
        }
        return mesh;
    }
};

class MeshLoadingHelper
{
public:
    static brayns::ModelPtr load(const brayns::TriangleMesh &mesh, brayns::Scene &scene)
    {
        auto model = scene.createModel();
        size_t defaultMaterialId = 0;
        model->createMaterial(defaultMaterialId, "default");
        auto &meshes = model->getTriangleMeshes();
        meshes[defaultMaterialId] = mesh;
        return model;
    }
};

class MeshMetadataBuilder
{
public:
    static brayns::ModelMetadata build(const brayns::TriangleMesh &mesh)
    {
        auto vertexCount = mesh.vertices.size();
        auto triangleCount = mesh.indices.size();
        return {{"meshes", "1"}, {"vertices", std::to_string(vertexCount)}, {"faces", std::to_string(triangleCount)}};
    }
};

class MeshImporter
{
public:
    static std::vector<brayns::ModelDescriptorPtr>
        import(brayns::Scene &scene, const brayns::TriangleMesh &mesh, const std::string &name)
    {
        auto model = MeshLoadingHelper::load(mesh, scene);
        auto metadata = MeshMetadataBuilder::build(mesh);
        auto descriptor = std::make_shared<brayns::ModelDescriptor>(std::move(model), name, metadata);
        return {descriptor};
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> MeshParserRegistry::getAllSupportedExtensions() const
{
    std::vector<std::string> extensions;
    extensions.reserve(_parsers.size());
    for (const auto &[extension, parser] : _parsers)
    {
        extensions.push_back(extension);
    }
    return extensions;
}

const MeshParser &MeshParserRegistry::getParser(const std::string &extension) const
{
    auto parser = findParser(extension);
    if (!parser)
    {
        throw std::runtime_error("Mesh format extension not supported: '" + extension + "'");
    }
    return *parser;
}

const MeshParser *MeshParserRegistry::findParser(const std::string &extension) const
{
    auto i = _parsers.find(extension);
    return i == _parsers.end() ? nullptr : i->second.get();
}

void MeshParserRegistry::addParser(std::shared_ptr<MeshParser> parser)
{
    auto extensions = parser->getSupportedExtensions();
    for (auto &extension : extensions)
    {
        auto [i, ok] = _parsers.emplace(std::move(extension), std::move(parser));
        if (!ok)
        {
            throw std::runtime_error("Mesh format extension supported by multiple parsers: '" + extension + "'");
        }
    }
}

MeshLoader::MeshLoader()
{
    _parsers.add<ObjMeshParser>();
    _parsers.add<PlyMeshParser>();
    _parsers.add<StlMeshParser>();
    _parsers.add<OffMeshParser>();
}

std::vector<std::string> MeshLoader::getSupportedExtensions() const
{
    return _parsers.getAllSupportedExtensions();
}

std::string MeshLoader::getName() const
{
    return "mesh";
}

std::vector<ModelDescriptorPtr>
    MeshLoader::importFromFile(const std::string &fileName, const LoaderProgress &callback, Scene &scene) const
{
    (void)callback;
    auto mesh = MeshParsingHelper::parse(_parsers, fileName);
    auto &name = fileName;
    return MeshImporter::import(scene, mesh, name);
}

std::vector<ModelDescriptorPtr> MeshLoader::importFromBlob(Blob &&blob, const LoaderProgress &callback, Scene &scene)
    const
{
    (void)callback;
    auto mesh = MeshParsingHelper::parse(_parsers, blob);
    auto &name = blob.name;
    return MeshImporter::import(scene, mesh, name);
}
} // namespace brayns

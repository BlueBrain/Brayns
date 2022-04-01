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

#include <brayns/io/loaders/mesh/MeshLoader.h>

#include <brayns/engine/components/GeometryRendererComponent.h>

#include <brayns/io/loaders/mesh/parsers/ObjMeshParser.h>
#include <brayns/io/loaders/mesh/parsers/OffMeshParser.h>
#include <brayns/io/loaders/mesh/parsers/PlyMeshParser.h>
#include <brayns/io/loaders/mesh/parsers/StlMeshParser.h>

#include <brayns/utils/FileReader.h>

#include <filesystem>
#include <fstream>
#include <sstream>

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
    static std::unique_ptr<brayns::Model> load(const brayns::TriangleMesh &mesh)
    {
        auto model = std::make_unique<brayns::Model>();

        model->addComponent<brayns::GeometryRendererComponent<brayns::TriangleMesh>>(mesh);

        return model;
    }
};

class MeshMetadataBuilder
{
public:
    static std::map<std::string, std::string> build(const brayns::TriangleMesh &mesh)
    {
        auto vertexCount = mesh.vertices.size();
        auto triangleCount = mesh.indices.size();
        return {{"meshes", "1"}, {"vertices", std::to_string(vertexCount)}, {"faces", std::to_string(triangleCount)}};
    }
};

class MeshImporter
{
public:
    static std::vector<std::unique_ptr<brayns::Model>> import(const brayns::TriangleMesh &mesh)
    {
        auto model = MeshLoadingHelper::load(mesh);

        auto metadata = MeshMetadataBuilder::build(mesh);
        model->setMetaData(metadata);

        std::vector<std::unique_ptr<brayns::Model>> result;
        result.push_back(std::move(model));
        return result;
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

std::vector<std::unique_ptr<Model>> MeshLoader::importFromFile(const std::string &fileName, const LoaderProgress &callback) const
{
    (void)callback;
    auto mesh = MeshParsingHelper::parse(_parsers, fileName);
    return MeshImporter::import(mesh);
}

std::vector<std::unique_ptr<Model>> MeshLoader::importFromBlob(Blob &&blob, const LoaderProgress &callback)
    const
{
    (void)callback;
    auto mesh = MeshParsingHelper::parse(_parsers, blob);
    return MeshImporter::import(mesh);
}
} // namespace brayns

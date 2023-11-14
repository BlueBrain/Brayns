/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/colormethods/PrimitiveColorMethod.h>
#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/Metadata.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <brayns/io/loaders/mesh/parsers/ObjMeshParser.h>
#include <brayns/io/loaders/mesh/parsers/OffMeshParser.h>
#include <brayns/io/loaders/mesh/parsers/PlyMeshParser.h>
#include <brayns/io/loaders/mesh/parsers/StlMeshParser.h>

#include <brayns/utils/FileReader.h>
#include <brayns/utils/parsing/Parser.h>
#include <brayns/utils/parsing/ParsingException.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringInfo.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
class MeshParsingHelper
{
public:
    static brayns::TriangleMesh parse(
        const brayns::MeshParserRegistry &parsers,
        const std::string &format,
        std::string_view data)
    {
        auto &parser = parsers.getParser(format);
        auto mesh = _parse(parser, data);
        if (mesh.indices.empty())
        {
            throw std::runtime_error("No meshes found");
        }
        return mesh;
    }

private:
    static brayns::TriangleMesh _parse(const brayns::MeshParser &parser, std::string_view data)
    {
        try
        {
            return parser.parse(data);
        }
        catch (const brayns::ParsingException &e)
        {
            throw std::runtime_error(e.format());
        }
    }
};

class MeshLoadingHelper
{
public:
    static std::shared_ptr<brayns::Model> load(const brayns::TriangleMesh &mesh)
    {
        auto model = std::make_shared<brayns::Model>("mesh");

        auto &components = model->getComponents();
        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.push_back(brayns::Geometry(mesh));

        auto colorMethods = std::vector<std::unique_ptr<brayns::IColorMethod>>();
        colorMethods.push_back(std::make_unique<brayns::SolidColorMethod>());
        colorMethods.push_back(std::make_unique<brayns::PrimitiveColorMethod>("triangle", mesh.indices.size()));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setDataSystem<brayns::GeometryDataSystem>();
        systems.setColorSystem<brayns::GenericColorSystem>(std::move(colorMethods));

        return model;
    }
};

class MeshMetadataBuilder
{
public:
    static brayns::Metadata build(const brayns::TriangleMesh &mesh)
    {
        auto vertexCount = mesh.vertices.size();
        auto triangleCount = mesh.indices.size();
        return brayns::Metadata{
            {"meshes", "1"},
            {"vertices", std::to_string(vertexCount)},
            {"faces", std::to_string(triangleCount)}};
    }
};

class MeshImporter
{
public:
    static std::vector<std::shared_ptr<brayns::Model>> import(const brayns::TriangleMesh &mesh)
    {
        auto model = MeshLoadingHelper::load(mesh);

        auto &components = model->getComponents();
        components.add<brayns::Metadata>(MeshMetadataBuilder::build(mesh));

        auto result = std::vector<std::shared_ptr<brayns::Model>>();
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

std::string MeshLoader::getName() const
{
    return "mesh";
}

std::vector<std::string> MeshLoader::getExtensions() const
{
    return _parsers.getAllSupportedExtensions();
}

std::vector<std::shared_ptr<Model>> MeshLoader::loadBinary(const BinaryRequest &request)
{
    auto format = std::string(request.format);
    auto mesh = MeshParsingHelper::parse(_parsers, format, request.data);
    return MeshImporter::import(mesh);
}
} // namespace brayns

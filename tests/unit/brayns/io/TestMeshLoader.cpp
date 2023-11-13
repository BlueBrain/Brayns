/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest/doctest.h>

#include <brayns/io/loaders/mesh/MeshLoader.h>
#include <brayns/utils/FileReader.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/paths.h>
#include <tests/unit/PlaceholderEngine.h>

#include <filesystem>

namespace
{
class MeshLoader
{
public:
    static std::vector<std::shared_ptr<brayns::Model>> loadFile(const std::string &path)
    {
        auto loader = brayns::MeshLoader();
        auto request = brayns::MeshLoader::FileRequest();
        request.path = path;
        return loader.loadFile(request);
    }

    static std::vector<std::shared_ptr<brayns::Model>> loadBinary(const std::string &path)
    {
        auto loader = brayns::MeshLoader();
        auto request = brayns::MeshLoader::BinaryRequest();
        request.format = brayns::LoaderFormat::from(path);
        request.data = brayns::FileReader::read(path);
        return loader.loadBinary(request);
    }
};

class MeshExtractor
{
public:
    static const brayns::TriangleMesh &extract(const brayns::Model &model)
    {
        auto &components = model.getComponents();
        auto geometries = components.find<brayns::Geometries>();
        CHECK(geometries);
        auto &geometry = geometries->elements.front();
        auto cast = geometry.as<brayns::TriangleMesh>();
        CHECK(cast);
        return cast->front();
    }
};
}

TEST_CASE("Mesh loader")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Name")
    {
        auto loader = brayns::MeshLoader();
        CHECK(loader.getName() == "mesh");
    }
    SUBCASE("Invalid mesh load")
    {
        auto invalidFormatMessage = "Mesh format extension not supported: 'fbx'";
        CHECK_THROWS_WITH(MeshLoader::loadFile(TestPaths::Meshes::fbx), invalidFormatMessage);

        auto corruptedMeshMessage = "No meshes found";
        CHECK_THROWS_WITH(MeshLoader::loadFile(TestPaths::Meshes::empty), corruptedMeshMessage);
    }
    SUBCASE("Loaded model properties")
    {
        auto loadedModels = MeshLoader::loadFile(TestPaths::Meshes::obj);
        auto &model = *loadedModels.front();
        CHECK(model.getType() == "mesh");
        MeshExtractor::extract(model);
    }
    SUBCASE("OBJ loading")
    {
        auto blobList = MeshLoader::loadBinary(TestPaths::Meshes::obj);
        auto fileList = MeshLoader::loadFile(TestPaths::Meshes::obj);

        CHECK(blobList.size() == 1);
        CHECK(fileList.size() == 1);

        auto &blob = *blobList.front();
        auto &file = *fileList.front();
        auto &blobMesh = MeshExtractor::extract(blob);
        auto &fileMesh = MeshExtractor::extract(file);

        CHECK(blobMesh.indices.size() == 24);
        CHECK(blobMesh.vertices.size() == 72);
        CHECK(blobMesh.uvs.size() == 72);
        CHECK(blobMesh.normals.size() == 72);
        CHECK(blobMesh.colors.size() == 0);

        CHECK(blobMesh.indices == fileMesh.indices);
        CHECK(blobMesh.vertices == fileMesh.vertices);
        CHECK(blobMesh.normals == fileMesh.normals);
        CHECK(blobMesh.uvs == fileMesh.uvs);
        CHECK(blobMesh.colors == fileMesh.colors);
    }
    SUBCASE("PLY loading")
    {
        auto blobList = MeshLoader::loadBinary(TestPaths::Meshes::ply);
        auto fileList = MeshLoader::loadFile(TestPaths::Meshes::ply);

        CHECK(blobList.size() == 1);
        CHECK(fileList.size() == 1);

        auto &blob = *blobList.front();
        auto &file = *fileList.front();
        auto &blobMesh = MeshExtractor::extract(blob);
        auto &fileMesh = MeshExtractor::extract(file);

        CHECK(blobMesh.indices.size() == 12);
        CHECK(blobMesh.vertices.size() == 36);
        CHECK(blobMesh.uvs.size() == 0);
        CHECK(blobMesh.normals.size() == 0);
        CHECK(blobMesh.colors.size() == 36);

        CHECK(blobMesh.indices == fileMesh.indices);
        CHECK(blobMesh.vertices == fileMesh.vertices);
        CHECK(blobMesh.normals == fileMesh.normals);
        CHECK(blobMesh.uvs == fileMesh.uvs);
        CHECK(blobMesh.colors == fileMesh.colors);
    }
    SUBCASE("OFF loading")
    {
        auto blobList = MeshLoader::loadBinary(TestPaths::Meshes::off);
        auto fileList = MeshLoader::loadFile(TestPaths::Meshes::off);

        CHECK(blobList.size() == 1);
        CHECK(fileList.size() == 1);

        auto &blob = *blobList.front();
        auto &file = *fileList.front();
        auto &blobMesh = MeshExtractor::extract(blob);
        auto &fileMesh = MeshExtractor::extract(file);

        CHECK(blobMesh.indices.size() == 12);
        CHECK(blobMesh.vertices.size() == 36);
        CHECK(blobMesh.uvs.size() == 0);
        CHECK(blobMesh.normals.size() == 0);
        CHECK(blobMesh.colors.size() == 0);

        CHECK(blobMesh.indices == fileMesh.indices);
        CHECK(blobMesh.vertices == fileMesh.vertices);
        CHECK(blobMesh.normals == fileMesh.normals);
        CHECK(blobMesh.uvs == fileMesh.uvs);
        CHECK(blobMesh.colors == fileMesh.colors);
    }
    SUBCASE("STL loading")
    {
        auto blobList = MeshLoader::loadBinary(TestPaths::Meshes::stl);
        auto fileList = MeshLoader::loadFile(TestPaths::Meshes::stl);

        CHECK(blobList.size() == 1);
        CHECK(fileList.size() == 1);

        auto &blob = *blobList.front();
        auto &file = *fileList.front();
        auto &blobMesh = MeshExtractor::extract(blob);
        auto &fileMesh = MeshExtractor::extract(file);

        CHECK(blobMesh.indices.size() == 12);
        CHECK(blobMesh.vertices.size() == 36);
        CHECK(blobMesh.uvs.size() == 0);
        CHECK(blobMesh.normals.size() == 36);
        CHECK(blobMesh.colors.size() == 0);

        CHECK(blobMesh.indices == fileMesh.indices);
        CHECK(blobMesh.vertices == fileMesh.vertices);
        CHECK(blobMesh.normals == fileMesh.normals);
        CHECK(blobMesh.uvs == fileMesh.uvs);
        CHECK(blobMesh.colors == fileMesh.colors);
    }
}

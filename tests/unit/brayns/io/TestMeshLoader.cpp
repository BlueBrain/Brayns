/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/paths.h>
#include <tests/unit/PlaceholderEngine.h>

namespace
{
class MeshExtractor
{
public:
    static const brayns::TriangleMesh &extract(const brayns::Model &model)
    {
        auto &components = model.getComponents();
        auto &geometries = components.get<brayns::Geometries>();
        auto &geometry = geometries.elements.front();
        auto cast = geometry.as<brayns::TriangleMesh>();
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
    SUBCASE("OBJ loading from blob")
    {
        auto loader = brayns::MeshLoader();
        auto blob = BraynsTestUtils::fileToBlob(TestPaths::Meshes::obj);
        auto result = loader.importFromBlob(blob, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 24);
        CHECK(mesh.vertices.size() == 72);
        CHECK(mesh.uvs.size() == 72);
        CHECK(mesh.normals.size() == 72);
        CHECK(mesh.colors.size() == 0);
    }
    SUBCASE("OBJ loading from file")
    {
        auto loader = brayns::MeshLoader();
        auto result = loader.importFromFile(TestPaths::Meshes::obj, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 24);
        CHECK(mesh.vertices.size() == 72);
        CHECK(mesh.uvs.size() == 72);
        CHECK(mesh.normals.size() == 72);
        CHECK(mesh.colors.size() == 0);
    }
    SUBCASE("PLY loading from blob")
    {
        auto loader = brayns::MeshLoader();
        auto blob = BraynsTestUtils::fileToBlob(TestPaths::Meshes::ply);
        auto result = loader.importFromBlob(blob, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 12);
        CHECK(mesh.vertices.size() == 36);
        CHECK(mesh.uvs.size() == 0);
        CHECK(mesh.normals.size() == 0);
        CHECK(mesh.colors.size() == 36);
    }
    SUBCASE("PLY loading from file")
    {
        auto loader = brayns::MeshLoader();
        auto result = loader.importFromFile(TestPaths::Meshes::ply, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 12);
        CHECK(mesh.vertices.size() == 36);
        CHECK(mesh.uvs.size() == 0);
        CHECK(mesh.normals.size() == 0);
        CHECK(mesh.colors.size() == 36);
    }
    SUBCASE("OFF loading from blob")
    {
        auto loader = brayns::MeshLoader();
        auto blob = BraynsTestUtils::fileToBlob(TestPaths::Meshes::off);
        auto result = loader.importFromBlob(blob, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 12);
        CHECK(mesh.vertices.size() == 36);
        CHECK(mesh.uvs.size() == 0);
        CHECK(mesh.normals.size() == 0);
        CHECK(mesh.colors.size() == 0);
    }
    SUBCASE("OFF loading from file")
    {
        auto loader = brayns::MeshLoader();
        auto result = loader.importFromFile(TestPaths::Meshes::off, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 12);
        CHECK(mesh.vertices.size() == 36);
        CHECK(mesh.uvs.size() == 0);
        CHECK(mesh.normals.size() == 0);
        CHECK(mesh.colors.size() == 0);
    }
    SUBCASE("STL loading from blob")
    {
        auto loader = brayns::MeshLoader();
        auto blob = BraynsTestUtils::fileToBlob(TestPaths::Meshes::stl);
        auto result = loader.importFromBlob(blob, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 12);
        CHECK(mesh.vertices.size() == 36);
        CHECK(mesh.uvs.size() == 0);
        CHECK(mesh.normals.size() == 36);
        CHECK(mesh.colors.size() == 0);
    }
    SUBCASE("STL loading from file")
    {
        auto loader = brayns::MeshLoader();
        auto result = loader.importFromFile(TestPaths::Meshes::stl, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        CHECK(model.getType() == "mesh");

        auto &mesh = MeshExtractor::extract(model);
        CHECK(mesh.indices.size() == 12);
        CHECK(mesh.vertices.size() == 36);
        CHECK(mesh.uvs.size() == 0);
        CHECK(mesh.normals.size() == 36);
        CHECK(mesh.colors.size() == 0);
    }
}

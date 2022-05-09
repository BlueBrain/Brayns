/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/Log.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/TriangleMesh.h>

namespace
{
/**
 * @brief Checks the sanity of the triangle mesh
 */
class TriangleMeshAttributeChecker
{
public:
    static void check(const brayns::TriangleMesh &mesh)
    {
        auto &indices = mesh.indices;
        auto &positions = mesh.vertices;

        if (indices.empty() || positions.empty())
        {
            throw std::invalid_argument("TriangleMeshes must provide at least indices and vertices");
        }

        auto &normals = mesh.normals;
        if (!normals.empty() && normals.size() != positions.size())
        {
            throw std::invalid_argument("TriangleMeshes must provide one normal per vertex");
        }

        auto &uvs = mesh.uvs;
        if (!uvs.empty() && uvs.size() != positions.size())
        {
            throw std::invalid_argument("TriangleMeshes must provide one uv coordinate per vertex");
        }

        auto &colors = mesh.colors;
        if (!colors.empty() && colors.size() != positions.size())
        {
            throw std::invalid_argument("TriangleMeshes must provide one color per vertex");
        }
    }
};

/**
 * @brief Commits vector of attributes to OSPRay
 */
class AttributeCommitter
{
public:
    template<typename T>
    static void commit(OSPGeometry handle, const std::vector<T> &data, OSPDataType dataType, const char *id)
    {
        auto buffer = brayns::DataHandler::shareBuffer(data, dataType);
        ospSetParam(handle, id, OSPDataType::OSP_DATA, &buffer.handle);
    }
};

template<typename T>
void mergeVectors(const std::vector<T> &src, std::vector<T> &dst) noexcept
{
    dst.insert(dst.end(), src.begin(), src.end());
}
} // namespace

namespace brayns
{
void TriangleMeshMerger::merge(const TriangleMesh &src, TriangleMesh &dst)
{
    auto &indices = dst.indices;
    auto &positions = dst.vertices;
    auto &normals = dst.normals;
    auto &uvs = dst.uvs;
    auto &colors = dst.colors;

    const auto numVertices = positions.size();

    const auto &srcIndices = src.indices;
    const auto &srcPositions = src.vertices;
    const auto &srcNormals = src.normals;
    const auto &srcUvs = src.uvs;
    const auto &srcColors = src.colors;

    mergeVectors(srcPositions, positions);
    mergeVectors(srcNormals, normals);
    mergeVectors(srcUvs, uvs);
    mergeVectors(srcColors, colors);

    const auto srcIndicesSize = srcIndices.size();
    const auto indicesSize = indices.size();
    const Vector3ui indexOffset(numVertices);
    indices.reserve(indicesSize + srcIndicesSize);
    for (const auto &srcIndex : srcIndices)
    {
        indices.push_back(srcIndex + indexOffset);
    }
}

void TriangleMeshNormalGenerator::generate(TriangleMesh &mesh)
{
    auto &indices = mesh.indices;
    auto &positions = mesh.vertices;
    auto &normals = mesh.normals;

    normals.resize(positions.size());
    std::vector<uint32_t> normalAccumulationCounter(positions.size());

    for (const auto &triangle : indices)
    {
        const auto a = triangle.x;
        const auto b = triangle.y;
        const auto c = triangle.z;

        const auto &aVertex = positions[a];
        const auto &bVertex = positions[b];
        const auto &cVertex = positions[c];

        normals[a] += glm::cross((bVertex - aVertex), (cVertex - aVertex));
        normals[b] += glm::cross((aVertex - bVertex), (cVertex - bVertex));
        normals[c] += glm::cross((aVertex - cVertex), (bVertex - cVertex));

        normalAccumulationCounter[a] += 1;
        normalAccumulationCounter[b] += 1;
        normalAccumulationCounter[c] += 1;
    }

    for (size_t i = 0; i < normals.size(); ++i)
    {
        const auto inv = 1.f / static_cast<float>(normalAccumulationCounter[i]);
        normals[i] *= inv;
        glm::normalize(normals[i]);
    }
}

std::string_view GeometryOSPRayID<TriangleMesh>::get()
{
    return "mesh";
}

void GeometryBoundsUpdater<TriangleMesh>::update(const TriangleMesh &mesh, const Matrix4f &matrix, Bounds &bounds)
{
    const auto &vertices = mesh.vertices;

    for (const auto &vertex : vertices)
    {
        auto point = Vector3f(matrix * Vector4f(vertex, 1.f));
        bounds.expand(point);
    }
}

void GeometryAddChecker<TriangleMesh>::check(
    const std::vector<TriangleMesh> &dstGeometryList,
    const TriangleMesh &inputMesh)
{
    if (!dstGeometryList.empty())
    {
        throw std::invalid_argument("Geometry<TriangleMesh> can hold only 1 mesh");
    }

    TriangleMeshAttributeChecker::check(inputMesh);
}

void GeometryAddChecker<TriangleMesh>::check(
    const std::vector<TriangleMesh> &dstGeometryList,
    const std::vector<TriangleMesh> &inputMeshList)
{
    if (!dstGeometryList.empty() || inputMeshList.size() > 1)
    {
        throw std::invalid_argument("Geometry<TriangleMesh> can hold only 1 mesh");
    }

    const auto &mesh = inputMeshList.front();
    TriangleMeshAttributeChecker::check(mesh);
}

void GeometryCommitter<TriangleMesh>::commit(OSPGeometry handle, const std::vector<TriangleMesh> &geometries)
{
    auto &mesh = geometries.front();

    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;
    auto &normals = mesh.normals;
    auto &texCoors = mesh.uvs;
    auto &colors = mesh.colors;

    AttributeCommitter::commit(handle, vertices, OSPDataType::OSP_VEC3F, "vertex.position");
    AttributeCommitter::commit(handle, indices, OSPDataType::OSP_VEC3UI, "index");

    if (!normals.empty())
    {
        AttributeCommitter::commit(handle, normals, OSPDataType::OSP_VEC3F, "vertex.normal");
    }

    if (!texCoors.empty())
    {
        AttributeCommitter::commit(handle, texCoors, OSPDataType::OSP_VEC2F, "vertex.texcoord");
    }

    if (!colors.empty())
    {
        AttributeCommitter::commit(handle, colors, OSPDataType::OSP_VEC4F, "vertex.color");
    }
}
}

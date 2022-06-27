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

#include "TriangleMesh.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

#include <ospray/ospray_cpp/Data.h>

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

class VectorMerger
{
public:
    template<typename T>
    static void merge(const std::vector<T> &src, std::vector<T> &dst) noexcept
    {
        dst.insert(dst.end(), src.begin(), src.end());
    }
};

struct TriangleMeshParameters
{
    inline static const std::string osprayName = "mesh";
    inline static const std::string position = "vertex.position";
    inline static const std::string index = "index";
    inline static const std::string normal = "vertex.normal";
    inline static const std::string uv = "vertex.texcoord";
    inline static const std::string color = "vertex.color";
};
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

    VectorMerger::merge(srcPositions, positions);
    VectorMerger::merge(srcNormals, normals);
    VectorMerger::merge(srcUvs, uvs);
    VectorMerger::merge(srcColors, colors);

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

const std::string &OsprayGeometryName<TriangleMesh>::get()
{
    return TriangleMeshParameters::osprayName;
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

void InputGeometryChecker<TriangleMesh>::check(const std::vector<TriangleMesh> &primitives)
{
    if (primitives.size() > 1)
    {
        throw std::invalid_argument("Geometry<TriangleMesh> can hold only 1 mesh");
    }

    TriangleMeshAttributeChecker::check(primitives.front());
}

void GeometryCommitter<TriangleMesh>::commit(
    const ospray::cpp::Geometry &osprayGeometry,
    const std::vector<TriangleMesh> &primitives)
{
    auto &mesh = primitives.front();

    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;
    auto &normals = mesh.normals;
    auto &uvs = mesh.uvs;
    auto &colors = mesh.colors;

    osprayGeometry.setParam(TriangleMeshParameters::position, ospray::cpp::SharedData(vertices));
    osprayGeometry.setParam(TriangleMeshParameters::index, ospray::cpp::SharedData(indices));
    if (!normals.empty())
    {
        osprayGeometry.setParam(TriangleMeshParameters::normal, ospray::cpp::SharedData(normals));
    }
    if (!uvs.empty())
    {
        osprayGeometry.setParam(TriangleMeshParameters::uv, ospray::cpp::SharedData(uvs));
    }
    if (!colors.empty())
    {
        osprayGeometry.setParam(TriangleMeshParameters::uv, ospray::cpp::SharedData(colors));
    }
}
}

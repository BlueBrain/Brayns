/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <limits>

#include <ospray/ospray_cpp/Data.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

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

        auto max = std::numeric_limits<brayns::Vector3ui::Scalar>::max();
        if (positions.size() > static_cast<size_t>(max))
        {
            throw std::invalid_argument("TriangleMeshes vertex count exceeded");
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
    static inline const std::string position = "vertex.position";
    static inline const std::string index = "index";
    static inline const std::string normal = "vertex.normal";
    static inline const std::string uv = "vertex.texcoord";
    static inline const std::string color = "vertex.color";
};
} // namespace

namespace brayns
{
void TriangleMeshUtils::merge(const TriangleMesh &src, TriangleMesh &dst)
{
    auto &indices = dst.indices;
    auto &positions = dst.vertices;
    auto &normals = dst.normals;
    auto &uvs = dst.uvs;
    auto &colors = dst.colors;

    const auto numVertices = static_cast<Vector3ui::Scalar>(positions.size());

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
    const auto indexOffset = Vector3ui(numVertices);
    indices.reserve(indicesSize + srcIndicesSize);
    for (const auto &srcIndex : srcIndices)
    {
        indices.push_back(srcIndex + indexOffset);
    }
}

void TriangleMeshUtils::generateNormals(TriangleMesh &mesh)
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

        normals[a] += math::cross((bVertex - aVertex), (cVertex - aVertex));
        normals[b] += math::cross((aVertex - bVertex), (cVertex - bVertex));
        normals[c] += math::cross((aVertex - cVertex), (bVertex - cVertex));

        normalAccumulationCounter[a] += 1;
        normalAccumulationCounter[b] += 1;
        normalAccumulationCounter[c] += 1;
    }

    for (size_t i = 0; i < normals.size(); ++i)
    {
        const auto inv = 1.f / static_cast<float>(normalAccumulationCounter[i]);
        normals[i] *= inv;
        math::normalize(normals[i]);
    }
}

Bounds GeometryTraits<TriangleMesh>::computeBounds(const TransformMatrix &matrix, const TriangleMesh &data)
{
    Bounds bounds;
    for (auto &vertex : data.vertices)
    {
        bounds.expand(matrix.transformPoint(vertex));
    }
    return bounds;
}

void GeometryTraits<TriangleMesh>::updateData(ospray::cpp::Geometry &handle, std::vector<TriangleMesh> &data)
{
    assert(data.size() == 1);

    auto &mesh = data.front();
    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;
    auto &normals = mesh.normals;
    auto &uvs = mesh.uvs;
    auto &colors = mesh.colors;

    handle.setParam(TriangleMeshParameters::position, ospray::cpp::SharedData(vertices));
    handle.setParam(TriangleMeshParameters::index, ospray::cpp::SharedData(indices));
    if (!normals.empty())
    {
        handle.setParam(TriangleMeshParameters::normal, ospray::cpp::SharedData(normals));
    }
    if (!uvs.empty())
    {
        handle.setParam(TriangleMeshParameters::uv, ospray::cpp::SharedData(uvs));
    }
    if (!colors.empty())
    {
        handle.setParam(TriangleMeshParameters::uv, ospray::cpp::SharedData(colors));
    }
}
}

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

#include <brayns/engine/geometries/TriangleMesh.h>

#include <brayns/common/Log.h>

namespace
{
template<typename T>
void commitVector(OSPGeometry handle, std::vector<T>& data, OSPDataType dataType, const char* id)
{
    auto sharedData = ospNewSharedData(data.data(), dataType, data.size());
    ospSetParam(handle, id, OSPDataType::OSP_DATA, &sharedData);
    ospRelease(sharedData);
}

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
    const Vector3ui indexOffset (numVertices);
    indices.reserve(indicesSize + srcIndicesSize);
    for(const auto &srcIndex : srcIndices)
    {
        indices.push_back(srcIndex + indexOffset);
    }
}

template<>
std::string_view RenderableOSPRayID<TriangleMesh>::get()
{
    return "mesh";
}

template<>
void RenderableBoundsUpdater<TriangleMesh>::update(const TriangleMesh& mesh, const Matrix4f& matrix, Bounds& bounds)
{
    const auto& vertices = mesh.vertices;

    for(const auto& vertex : vertices)
    {
        auto point = Vector3f(matrix * Vector4f(vertex, 1.f));
        bounds.expand(point);
    }
}

template<>
uint32_t Geometry<TriangleMesh>::add(TriangleMesh geometry)
{
    if(!_geometries.empty())
    {
        throw std::runtime_error("TriangleMesh Geometry can only handle 1 mesh");
    }

    if(geometry.vertices.empty())
    {
        throw std::invalid_argument("TriangleMesh must provide vertex positions");
    }

    if(geometry.indices.empty())
    {
        throw std::invalid_argument("TriangleMesh must provide vertex indices");
    }

    _geometries.push_back(std::move(geometry));
    _geometries.shrink_to_fit();
    _dirty = true;
    return 0;
}

template<>
std::vector<uint32_t> Geometry<TriangleMesh>::add(const std::vector<TriangleMesh>& geometries)
{
    (void) geometries;
    throw std::runtime_error("TriangleMesh Geometry can only handle 1 mesh. Use add() to add a single mesh");
    return {};
}

template<>
std::vector<uint32_t> Geometry<TriangleMesh>::set(std::vector<TriangleMesh> geometries)
{
    (void) geometries;
    throw std::runtime_error("TriangleMesh Geometry can only handle 1 mesh. Use add() to add a single mesh");
    return {};
}

template<>
void Geometry<TriangleMesh>::commitGeometrySpecificParams()
{
    auto& mesh = _geometries[0];

    auto& vertices = mesh.vertices;
    auto& indices = mesh.indices;
    auto& normals = mesh.normals;
    auto& texCoors = mesh.uvs;
    auto& colors = mesh.colors;

    commitVector(_handle, vertices, OSPDataType::OSP_VEC3F, "vertex.position");
    commitVector(_handle, indices, OSPDataType::OSP_VEC3UI, "index");

    if(!normals.empty())
    {
        if(normals.size() != vertices.size())
        {
            Log::warn("TriangleMesh does not have a normal vector per vertex. Skipping");
        }
        else
        {
            commitVector(_handle, normals, OSPDataType::OSP_VEC3F, "vertex.normal");
        }
    }

    if(!texCoors.empty())
    {
        if(texCoors.size() != vertices.size())
        {
            Log::warn("TriangleMesh does not have an UV coordinate per vertex. Skipping");
        }
        else
        {
            commitVector(_handle, texCoors, OSPDataType::OSP_VEC2F, "vertex.texcoord");
        }
    }

    if(!colors.empty())
    {
        if(colors.size() != vertices.size())
        {
            Log::warn("TriangleMesh does not have a color per vertex. Skipping");
        }
        else
        {
            commitVector(_handle, colors, OSPDataType::OSP_VEC4F, "vertex.color");
        }
    }
}
}

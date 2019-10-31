/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <brayns/common/types.h>

namespace brayns
{
struct TriangleMesh
{
    Vector3fs vertices;
    Vector3fs normals;
    Vector4fs colors;
    std::vector<Vector3ui> indices;
    std::vector<Vector2f> textureCoordinates;
};

inline TriangleMesh createBox(const Vector3f& minCorner, const Vector3f& maxCorner)
{
    TriangleMesh result;
    const size_t numVertices = 24;
    const size_t numFaces = 12;
    result.vertices.reserve(numVertices);
    result.normals.reserve(numVertices);
    result.indices.reserve(numFaces);
    // Front face
    result.vertices.emplace_back(minCorner.x, maxCorner.y, maxCorner.z);
    result.vertices.emplace_back(maxCorner);
    result.vertices.emplace_back(minCorner.x, minCorner.y, maxCorner.z);
    result.vertices.emplace_back(maxCorner.x, minCorner.y, maxCorner.z);
    result.normals.emplace_back(0.f, 0.f, -1.f);
    result.normals.emplace_back(0.f, 0.f, -1.f);
    result.normals.emplace_back(0.f, 0.f, -1.f);
    result.normals.emplace_back(0.f, 0.f, -1.f);
    result.indices.emplace_back(0, 2, 1);
    result.indices.emplace_back(1, 2, 3);
    // Back face
    result.vertices.emplace_back(minCorner.x, maxCorner.y, minCorner.z);
    result.vertices.emplace_back(maxCorner.x, maxCorner.y, minCorner.z);
    result.vertices.emplace_back(minCorner);
    result.vertices.emplace_back(maxCorner.x, minCorner.y, minCorner.z);
    result.normals.emplace_back(0.f, 0.f, 1.f);
    result.normals.emplace_back(0.f, 0.f, 1.f);
    result.normals.emplace_back(0.f, 0.f, 1.f);
    result.normals.emplace_back(0.f, 0.f, 1.f);
    result.indices.emplace_back(4, 6, 5);
    result.indices.emplace_back(5, 6, 7);
    // Left face
    result.vertices.emplace_back(minCorner.x, maxCorner.y, minCorner.z);
    result.vertices.emplace_back(minCorner.x, maxCorner.y, maxCorner.z);
    result.vertices.emplace_back(minCorner);
    result.vertices.emplace_back(minCorner.x, minCorner.y, maxCorner.z);
    result.normals.emplace_back(-1.f, 0.f, 0.f);
    result.normals.emplace_back(-1.f, 0.f, 0.f);
    result.normals.emplace_back(-1.f, 0.f, 0.f);
    result.normals.emplace_back(-1.f, 0.f, 0.f);
    result.indices.emplace_back(8, 10, 9);
    result.indices.emplace_back(9, 10, 11);
    // Right face
    result.vertices.emplace_back(maxCorner);
    result.vertices.emplace_back(maxCorner.x, maxCorner.y, minCorner.z);
    result.vertices.emplace_back(maxCorner.x, minCorner.y, maxCorner.z);
    result.vertices.emplace_back(maxCorner.x, minCorner.y, minCorner.z);
    result.normals.emplace_back(1.f, 0.f, 0.f);
    result.normals.emplace_back(1.f, 0.f, 0.f);
    result.normals.emplace_back(1.f, 0.f, 0.f);
    result.normals.emplace_back(1.f, 0.f, 0.f);
    result.indices.emplace_back(12, 14, 13);
    result.indices.emplace_back(13, 14, 15);
    // Top face
    result.vertices.emplace_back(minCorner.x, maxCorner.y, minCorner.z);
    result.vertices.emplace_back(maxCorner.x, maxCorner.y, minCorner.z);
    result.vertices.emplace_back(minCorner.x, maxCorner.y, maxCorner.z);
    result.vertices.emplace_back(maxCorner);
    result.normals.emplace_back(0.f, 1.f, 0.f);
    result.normals.emplace_back(0.f, 1.f, 0.f);
    result.normals.emplace_back(0.f, 1.f, 0.f);
    result.normals.emplace_back(0.f, 1.f, 0.f);
    result.indices.emplace_back(16, 18, 17);
    result.indices.emplace_back(17, 18, 19);
    // Bottom face
    result.vertices.emplace_back(maxCorner.x, minCorner.y, minCorner.z);
    result.vertices.emplace_back(minCorner);
    result.vertices.emplace_back(maxCorner.x, minCorner.y, maxCorner.z);
    result.vertices.emplace_back(minCorner.x, minCorner.y, maxCorner.z);
    result.normals.emplace_back(0.f, -1.f, 0.f);
    result.normals.emplace_back(0.f, -1.f, 0.f);
    result.normals.emplace_back(0.f, -1.f, 0.f);
    result.normals.emplace_back(0.f, -1.f, 0.f);
    result.indices.emplace_back(20, 22, 21);
    result.indices.emplace_back(21, 22, 23);

    return result;
}

}

#endif // TRIANGLEMESH_H

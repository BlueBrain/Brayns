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

#pragma once

#include <brayns/engine/geometry/GeometryTraits.h>

namespace brayns
{
struct TriangleMesh
{
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector4f> colors;
    std::vector<Vector2f> uvs;
    std::vector<Vector3ui> indices;
};

class TriangleMeshUtils
{
public:
    static void merge(const TriangleMesh &src, TriangleMesh &dst);
    static void generateNormals(TriangleMesh &mesh);
};

template<>
class GeometryTraits<TriangleMesh>
{
public:
    static inline const std::string handleName = "mesh";
    static inline const std::string name = "mesh";

    static Bounds computeBounds(const TransformMatrix &matrix, const TriangleMesh &data);
    static void updateData(ospray::cpp::Geometry &handle, std::vector<TriangleMesh> &data);
};
}

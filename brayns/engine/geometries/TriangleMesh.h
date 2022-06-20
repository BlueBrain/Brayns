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

#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Geometry.h>

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

class TriangleMeshMerger
{
public:
    static void merge(const TriangleMesh &src, TriangleMesh &dst);
};

class TriangleMeshNormalGenerator
{
public:
    static void generate(TriangleMesh &mesh);
};

template<>
class OsprayGeometryName<TriangleMesh>
{
public:
    static std::string_view get();
};

template<>
class GeometryBoundsUpdater<TriangleMesh>
{
public:
    static void update(const TriangleMesh &mesh, const Matrix4f &matrix, Bounds &bounds);
};

template<>
class InputGeometryChecker<TriangleMesh>
{
public:
    static void check(const std::vector<TriangleMesh> &primitives);
};

template<>
class GeometryCommitter<TriangleMesh>
{
public:
    static void commit(const ospray::cpp::Geometry &osprayGeometry, const std::vector<TriangleMesh> &primitives);
};
}

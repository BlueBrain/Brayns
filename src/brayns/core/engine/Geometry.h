/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <variant>
#include <vector>

#include "Device.h"
#include "Object.h"
#include "Volume.h"

namespace brayns
{
class Geometry : public Managed<OSPGeometry>
{
public:
    using Managed::Managed;
};

struct MeshSettings
{
    std::vector<Vector3> positions;
    std::vector<Vector3> normals = {};
    std::vector<Color4> colors = {};
    std::vector<Vector2> uvs = {};
};

class Mesh : public Geometry
{
public:
    using Geometry::Geometry;
};

class TriangleMesh : public Mesh
{
public:
    using Mesh::Mesh;
};

TriangleMesh createTriangleMesh(Device &device, const MeshSettings &settings, const std::vector<Index3> &indices = {});

class QuadMesh : public Mesh
{
public:
    using Mesh::Mesh;
};

QuadMesh createQuadMesh(Device &device, const MeshSettings &settings, const std::vector<Index4> &indices = {});

struct SphereSettings
{
    std::vector<Vector4> positionsRadii;
    std::vector<Vector2> uvs = {};
};

class Spheres : public Geometry
{
public:
    using Geometry::Geometry;
};

Spheres createSpheres(Device &device, const SphereSettings &settings);

class Discs : public Geometry
{
public:
    using Geometry::Geometry;
};

Discs createDiscs(Device &device, const SphereSettings &settings, const std::vector<Vector3> &normals = {});

struct CurveSettings
{
    std::vector<Vector4> positionsRadii;
    std::vector<std::uint32_t> indices;
    std::vector<Color4> colors = {};
    std::vector<Vector2> uvs = {};
};

enum class CurveType
{
    Flat = OSP_FLAT,
    Round = OSP_ROUND,
};

struct LinearBasis
{
};

struct BezierBasis
{
};

struct BsplineBasis
{
};

struct HermiteBasis
{
    std::vector<Vector4> tangents;
};

struct CatmullRomBasis
{
};

using CurveBasis = std::variant<LinearBasis, BezierBasis, BsplineBasis, HermiteBasis, CatmullRomBasis>;
using RibbonBasis = std::variant<BezierBasis, BsplineBasis, HermiteBasis, CatmullRomBasis>;

class Cylinders : public Geometry
{
public:
    using Geometry::Geometry;
};

Cylinders createCylinders(Device &device, const CurveSettings &settings);

class Curve : public Geometry
{
public:
    using Geometry::Geometry;
};

Curve createCurve(Device &device, const CurveSettings &settings, CurveType type = CurveType::Flat, const CurveBasis &basis = LinearBasis());

class Ribbon : public Geometry
{
public:
    using Geometry::Geometry;
};

Ribbon createRibbon(Device &device, const CurveSettings &settings, const std::vector<Vector3> &normals, const RibbonBasis &basis = BezierBasis());

struct BoxSettings
{
    std::vector<Box3> boxes;
};

class Boxes : public Geometry
{
public:
    using Geometry::Geometry;
};

Boxes createBoxes(Device &device, const BoxSettings &settings);

struct PlaneSettings
{
    std::vector<Vector4> coefficients;
    std::vector<Box3> bounds = {};
};

class Planes : public Geometry
{
public:
    using Geometry::Geometry;
};

Planes createPlanes(Device &device, const PlaneSettings &settings);

struct IsosurfaceSettings
{
    std::vector<float> isovalues;
};

class Isosurfaces : public Geometry
{
public:
    using Geometry::Geometry;
};

Isosurfaces createIsosurfaces(Device &device, const Volume &volume, const IsosurfaceSettings &settings);
}

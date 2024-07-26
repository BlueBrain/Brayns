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

#include <optional>
#include <variant>

#include "Data.h"
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
    Data<Vector3> positions;
    std::optional<Data<Vector3>> normals = std::nullopt;
    std::optional<Data<Color4>> colors = std::nullopt;
    std::optional<Data<Vector2>> uvs = std::nullopt;
};

class Mesh : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(const Data<Color4> &colors);
};

struct TriangleMeshSettings
{
    MeshSettings base;
    std::optional<Data<Index3>> indices = std::nullopt;
};

class TriangleMesh : public Mesh
{
public:
    using Mesh::Mesh;
};

TriangleMesh createTriangleMesh(Device &device, const TriangleMeshSettings &settings);

struct QuadMeshSettings
{
    MeshSettings base;
    std::optional<Data<Index4>> indices = std::nullopt;
};

class QuadMesh : public Mesh
{
public:
    using Mesh::Mesh;
};

QuadMesh createQuadMesh(Device &device, const QuadMeshSettings &settings);

struct SphereSettings
{
    Data<Vector4> spheres;
    std::optional<Data<Vector2>> uvs = std::nullopt;
};

class Spheres : public Geometry
{
public:
    using Geometry::Geometry;
};

Spheres createSpheres(Device &device, const SphereSettings &settings);

struct DiscSettings
{
    Data<Vector4> spheres;
    std::optional<Data<Vector3>> normals = std::nullopt;
    std::optional<Data<Vector2>> uvs = std::nullopt;
};

class Discs : public Geometry
{
public:
    using Geometry::Geometry;
};

Discs createDiscs(Device &device, const DiscSettings &settings);

struct CylinderSettings
{
    Data<Vector4> spheres;
    Data<std::uint32_t> indices;
    std::optional<Data<Color4>> colors = std::nullopt;
    std::optional<Data<Vector2>> uvs = std::nullopt;
};

class Cylinders : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(const Data<Color4> &colors);
};

Cylinders createCylinders(Device &device, const CylinderSettings &settings);

struct FlatCurve
{
};

struct RoundCurve
{
};

struct RibbonCurve
{
    Data<Vector3> normals;
};

using CurveType = std::variant<FlatCurve, RoundCurve, RibbonCurve>;

struct LinearCurve
{
};

struct BezierCurve
{
};

struct BsplineCurve
{
};

struct HermiteCurve
{
    Data<Vector4> tangents;
};

struct CatmullRomCurve
{
};

using CurveBasis = std::variant<LinearCurve, BezierCurve, BsplineCurve, HermiteCurve, CatmullRomCurve>;

struct CurveSettings
{
    Data<Vector4> spheres;
    Data<std::uint32_t> indices;
    std::optional<Data<Color4>> colors = std::nullopt;
    std::optional<Data<Vector2>> uvs = std::nullopt;
    CurveType type = RoundCurve();
    CurveBasis basis = LinearCurve();
};

class Curve : public Geometry
{
public:
    using Geometry::Geometry;
};

Curve createCurve(Device &device, const CurveSettings &settings);

struct BoxSettings
{
    Data<Box3> boxes;
};

class Boxes : public Geometry
{
public:
    using Geometry::Geometry;
};

Boxes createBoxes(Device &device, const BoxSettings &settings);

struct PlaneSettings
{
    Data<Vector4> coefficients;
    std::optional<Data<Box3>> bounds = std::nullopt;
};

class Planes : public Geometry
{
public:
    using Geometry::Geometry;
};

Planes createPlanes(Device &device, const PlaneSettings &settings);

struct IsosurfaceSettings
{
    Volume volume;
    Data<float> isovalues;
};

class Isosurfaces : public Geometry
{
public:
    using Geometry::Geometry;
};

Isosurfaces createIsosurfaces(Device &device, const IsosurfaceSettings &settings);
}

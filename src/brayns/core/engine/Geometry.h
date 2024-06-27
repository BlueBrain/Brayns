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

#include "Data.h"
#include "Object.h"
#include "Volume.h"

namespace brayns
{
class Geometry : public Managed<OSPGeometry>
{
public:
    using Managed::Managed;
};

struct MeshVertices
{
    std::span<Vector3> positions;
    std::span<Vector3> normals = {};
    std::span<Color4> colors = {};
    std::span<Vector2> uvs = {};
};

class Mesh : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(std::span<Color4> colors);
};

struct TriangleMeshSettings
{
    MeshVertices vertices;
    std::span<Index3> indices = {};
};

class TriangleMesh : public Mesh
{
public:
    using Mesh::Mesh;
};

template<>
struct ObjectReflector<TriangleMesh>
{
    using Settings = TriangleMeshSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct QuadMeshSettings
{
    MeshVertices vertices;
    std::span<Index4> indices = {};
};

class QuadMesh : public Mesh
{
public:
    using Mesh::Mesh;
};

template<>
struct ObjectReflector<QuadMesh>
{
    using Settings = QuadMeshSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

using PositionRadius = Vector4;

struct SphereSettings
{
    std::span<PositionRadius> points;
    std::span<Vector2> uvs = {};
};

class Spheres : public Geometry
{
public:
    using Geometry::Geometry;
};

template<>
struct ObjectReflector<Spheres>
{
    using Settings = SphereSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct DiscSettings
{
    std::span<PositionRadius> points;
    std::span<Vector3> normals = {};
    std::span<Vector2> uvs = {};
};

class Discs : public Geometry
{
public:
    using Geometry::Geometry;
};

template<>
struct ObjectReflector<Discs>
{
    using Settings = DiscSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct CurveVertices
{
    std::span<PositionRadius> points;
    std::span<Color4> colors = {};
    std::span<Vector2> uvs = {};
};

struct CylinderSettings
{
    CurveVertices vertices;
    std::span<std::uint32_t> indices;
};

class Cylinders : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(std::span<Color4> colors);
};

template<>
struct ObjectReflector<Cylinders>
{
    using Settings = CylinderSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct FlatCurve
{
};

struct RoundCurve
{
};

struct RibbonCurve
{
    std::span<Vector3> normals;
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
    std::span<Vector4> tangents;
};

struct CatmullRomCurve
{
};

using CurveBasis = std::variant<LinearCurve, BezierCurve, BsplineCurve, HermiteCurve, CatmullRomCurve>;

struct CurveSettings
{
    CurveVertices vertices;
    std::span<std::uint32_t> indices;
    CurveType type = RoundCurve();
    CurveBasis basis = LinearCurve();
};

class Curve : public Geometry
{
public:
    using Geometry::Geometry;
};

template<>
struct ObjectReflector<Curve>
{
    using Settings = CurveSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct BoxSettings
{
    std::span<Box3> boxes;
};

class Boxes : public Geometry
{
public:
    using Geometry::Geometry;
};

template<>
struct ObjectReflector<Boxes>
{
    using Settings = BoxSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct PlaneSettings
{
    std::span<Vector4> coefficients;
    std::span<Box3> bounds;
};

class Planes : public Geometry
{
public:
    using Geometry::Geometry;
};

template<>
struct ObjectReflector<Planes>
{
    using Settings = PlaneSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

struct IsosurfaceSettings
{
    Volume volume;
    std::span<float> isovalues;
};

class Isosurfaces : public Geometry
{
public:
    using Geometry::Geometry;
};

template<>
struct ObjectReflector<Isosurfaces>
{
    using Settings = IsosurfaceSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};
}

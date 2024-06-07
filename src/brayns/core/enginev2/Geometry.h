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

#include "Data.h"
#include "Object.h"
#include "Volume.h"

namespace brayns::experimental
{
class Geometry : public Managed<OSPGeometry>
{
public:
    using Managed::Managed;
};

struct MeshSettings
{
    std::span<Vector3> positions;
    std::span<Vector3> normals = {};
    std::span<Color4> colors = {};
    std::span<Vector2> uvs = {};
    std::span<Index3> indices = {};
};

class Mesh : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(std::span<Color4> colors);
};

struct TriangleMeshSettings : MeshSettings
{
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

struct QuadMeshSettings : MeshSettings
{
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

struct SphereSettings
{
    std::span<Vector3> positions;
    std::span<float> radii;
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

struct DiscSettings : SphereSettings
{
    std::span<Vector3> normals;
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

using PositionRadius = Vector4;

struct CylinderSettings
{
    std::span<PositionRadius> samples;
    std::span<std::uint32_t> indices;
    std::span<Color4> colors = {};
    std::span<Vector2> uvs = {};
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

enum class CurveType
{
    Flat = OSP_FLAT,
    Round = OSP_ROUND,
};

enum class CurveBasis
{
    Linear = OSP_LINEAR,
    Bezier = OSP_BEZIER,
    Bspline = OSP_BSPLINE,
    CatmullRom = OSP_CATMULL_ROM,
};

struct CurveSettings : CylinderSettings
{
    CurveType type = CurveType::Round;
    CurveBasis basis = CurveBasis::Linear;
};

class Curve : public Cylinders
{
public:
    using Cylinders::Cylinders;
};

template<>
struct ObjectReflector<Curve>
{
    using Settings = CurveSettings;

    static OSPGeometry createHandle(OSPDevice device, const Settings &settings);
};

enum class RibbonBasis
{
    Bezier = OSP_BEZIER,
    Bspline = OSP_BSPLINE,
    CatmullRom = OSP_CATMULL_ROM,
};

struct RibbonSettings : CylinderSettings
{
    RibbonBasis basis = RibbonBasis::Bezier;
    std::span<Vector3> normals;
};

class Ribbon : public Cylinders
{
public:
    using Cylinders::Cylinders;
};

template<>
struct ObjectReflector<Ribbon>
{
    using Settings = RibbonSettings;

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

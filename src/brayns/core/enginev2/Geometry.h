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

struct TriangleMeshSettings : MeshSettings
{
    std::span<Index3> indices = {};
};

void loadMeshParams(OSPGeometry handle, const TriangleMeshSettings &settings);

struct QuadMeshSettings : MeshSettings
{
    std::span<Index4> indices = {};
};

void loadMeshParams(OSPGeometry handle, const QuadMeshSettings &settings);

class Mesh : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(std::span<Color4> colors);
};

struct SphereSettings
{
    std::span<Vector3> positions;
    std::span<float> radii;
    std::span<Vector2> uvs = {};
};

void loadSphereParams(OSPGeometry handle, const SphereSettings &settings);

class Spheres : public Geometry
{
public:
    using Geometry::Geometry;
};

struct DiscSettings : SphereSettings
{
    std::span<Vector3> normals;
};

void loadDiscParams(OSPGeometry handle, const DiscSettings &settings);

class Discs : public Geometry
{
public:
    using Geometry::Geometry;
};

using PositionRadius = Vector4;

struct CylinderSettings
{
    std::span<PositionRadius> samples;
    std::span<std::uint32_t> indices;
    std::span<Color4> colors = {};
    std::span<Vector2> uvs = {};
};

void loadCylinderParams(OSPGeometry handle, const CylinderSettings &settings);

class Cylinders : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(std::span<Color4> colors);
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

void loadCurveParams(OSPGeometry handle, const CurveSettings &settings);

class Curve : public Cylinders
{
public:
    using Cylinders::Cylinders;
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

void loadRibbonParams(OSPGeometry handle, const RibbonSettings &settings);

class Ribbon : public Cylinders
{
public:
    using Cylinders::Cylinders;
};

struct BoxSettings
{
    std::span<Box3> boxes;
};

void loadBoxParams(OSPGeometry handle, const BoxSettings &settings);

class Boxes : public Geometry
{
public:
    using Geometry::Geometry;
};

struct PlaneSettings
{
    std::span<Vector4> coefficients;
    std::span<Box3> bounds;
};

void loadPlaneParams(OSPGeometry handle, const PlaneSettings &settings);

class Planes : public Geometry
{
public:
    using Geometry::Geometry;
};

struct IsosurfaceSettings
{
    Volume volume;
    std::span<float> isovalues;
};

void loadIsosurfaceParams(OSPGeometry handle, const IsosurfaceSettings &settings);

class Isosurfaces : public Geometry
{
public:
    using Geometry::Geometry;
};
}

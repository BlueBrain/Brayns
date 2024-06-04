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
    Data1D<Vector3> positions;
    std::optional<Data1D<Vector3>> normals = std::nullopt;
    std::optional<Data1D<Color4>> colors = std::nullopt;
    std::optional<Data1D<Vector2>> uvs = std::nullopt;
    std::optional<Data1D<Index3>> indices = std::nullopt;
};

struct TriangleMeshSettings : MeshSettings
{
    std::optional<Data1D<Index3>> indices = std::nullopt;
};

struct QuadMeshSettings : MeshSettings
{
    std::optional<Data1D<Index4>> indices = std::nullopt;
};

void loadMeshParams(OSPGeometry handle, const TriangleMeshSettings &settings);
void loadMeshParams(OSPGeometry handle, const QuadMeshSettings &settings);

class Mesh : public Geometry
{
public:
    using Geometry::Geometry;

    void setColors(const Data1D<Color4> &colors);
};

struct SphereSettings
{
    Data1D<Vector3> positions;
    std::variant<float, Data1D<float>> radii;
    std::optional<Data1D<Vector2>> uvs = std::nullopt;
};

struct DiscSettings : SphereSettings
{
    std::optional<Data1D<Vector3>> normals;
};

void loadSphereParams(OSPGeometry handle, const SphereSettings &settings);
void loadSphereParams(OSPGeometry handle, const DiscSettings &settings);

class Spheres : public Geometry
{
public:
    using Geometry::Geometry;

    void setRadii(const Data1D<float> &radii);
    void setRadius(float radius);
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

using PositionRadius = Vector4;

struct CylinderSettings
{
    Data1D<PositionRadius> samples;
    Data1D<std::uint32_t> indices;
    std::optional<Data1D<Color4>> colors = std::nullopt;
    std::optional<Data1D<Vector2>> uvs = std::nullopt;
};

struct CurveSettings : CylinderSettings
{
    CurveType type = CurveType::Round;
    CurveBasis basis = CurveBasis::Linear;
};

struct RibbonCurveSettings : CylinderSettings
{
    CurveBasis basis = CurveBasis::Bezier;
    Data1D<Vector3> normals;
};

struct HermiteCurveSettings : CylinderSettings
{
    CurveType type = CurveType::Round;
    Data1D<Vector4> tangents;
};

void loadCurveParams(OSPGeometry handle, const CylinderSettings &settings);
void loadCurveParams(OSPGeometry handle, const CurveSettings &settings);
void loadCurveParams(OSPGeometry handle, const RibbonCurveSettings &settings);
void loadCurveParams(OSPGeometry handle, const HermiteCurveSettings &settings);

class Curve : public Geometry
{
public:
    using Geometry::Geometry;

    void setSamples(const Data1D<PositionRadius> &samples);
    void setColors(const Data1D<Color4> &colors);
};

struct BoxSettings
{
    Data1D<Box3> boxes;
};

void loadBoxParams(OSPGeometry handle, const BoxSettings &settings);

class Boxes : public Geometry
{
public:
    using Geometry::Geometry;
};

struct PlaneSettings
{
    Data1D<Vector4> coefficients;
    std::optional<Data1D<Box3>> bounds;
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
    std::variant<float, Data1D<float>> isovalues;
};

void loadIsosurfaceParams(OSPGeometry handle, const IsosurfaceSettings &settings);

class Isosurfaces : public Geometry
{
public:
    using Geometry::Geometry;
};
}

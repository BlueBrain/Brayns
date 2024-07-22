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

#include "Geometry.h"

#include <stdexcept>

namespace
{
using namespace brayns;

void setMeshParams(OSPGeometry handle, const MeshSettings &settings)
{
    setObjectData(handle, "vertex.position", settings.positions);
    setObjectDataIfNotEmpty(handle, "vertex.normal", settings.normals);
    setObjectDataIfNotEmpty(handle, "vertex.color", settings.colors);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", settings.uvs);
}

// TODO remove when OSPRay exposes sphere.position_radius to match embree internal layout
void setInterleavedSpheresParams(OSPGeometry handle, std::span<Vector4> positionsAndRadii)
{
    constexpr auto stride = sizeof(Vector4);

    auto size = positionsAndRadii.size();

    const auto *positions = positionsAndRadii.data();
    const auto *radii = &positions[0].w;

    auto positionData = Data(ospNewSharedData(positions, OSP_VEC3F, size, stride));
    setObjectParam(handle, "sphere.position", positionData);

    auto radiusData = Data(ospNewSharedData(radii, OSP_FLOAT, size, stride));
    setObjectParam(handle, "sphere.radius", radiusData);
}

void setCurveType(OSPGeometry handle, FlatCurve)
{
    setObjectParam(handle, "type", OSP_FLAT);
}

void setCurveType(OSPGeometry handle, RoundCurve)
{
    setObjectParam(handle, "type", OSP_ROUND);
}

void setCurveType(OSPGeometry handle, const RibbonCurve &curve)
{
    setObjectParam(handle, "type", OSP_RIBBON);
    setObjectData(handle, "vertex.normal", curve.normals);
}

void setCurveBasis(OSPGeometry handle, LinearCurve)
{
    setObjectParam(handle, "basis", OSP_LINEAR);
}

void setCurveBasis(OSPGeometry handle, BezierCurve)
{
    setObjectParam(handle, "basis", OSP_BEZIER);
}

void setCurveBasis(OSPGeometry handle, BsplineCurve)
{
    setObjectParam(handle, "basis", OSP_BSPLINE);
}

void setCurveBasis(OSPGeometry handle, const HermiteCurve &curve)
{
    setObjectParam(handle, "basis", OSP_HERMITE);
    setObjectData(handle, "vertex.tangent", curve.tangents);
}

void setCurveBasis(OSPGeometry handle, CatmullRomCurve)
{
    setObjectParam(handle, "basis", OSP_CATMULL_ROM);
}
}

namespace brayns
{
void Mesh::setColors(std::span<Color4> colors)
{
    auto handle = getHandle();
    setObjectDataIfNotEmpty(handle, "vertex.color", colors);
    commitObject(handle);
}

void Cylinders::setColors(std::span<Color4> colors)
{
    auto handle = getHandle();
    setObjectDataIfNotEmpty(handle, "vertex.color", colors);
    commitObject(handle);
}

TriangleMesh createTriangleMesh(Device &device, const TriangleMeshSettings &settings)
{
    auto handle = ospNewGeometry("mesh");
    auto mesh = wrapObjectHandleAs<TriangleMesh>(device, handle);

    setMeshParams(handle, settings.base);
    setObjectDataIfNotEmpty(handle, "index", settings.indices);

    commitObject(handle);

    return mesh;
}

QuadMesh createQuadMesh(Device &device, const QuadMeshSettings &settings)
{
    auto handle = ospNewGeometry("mesh");
    auto mesh = wrapObjectHandleAs<QuadMesh>(device, handle);

    setMeshParams(handle, settings.base);

    if (settings.indices.empty())
    {
        setObjectParam(handle, "quadSoup", true);
    }
    else
    {
        setObjectData(handle, "index", settings.indices);
    }

    commitObject(handle);

    return mesh;
}

Spheres createSpheres(Device &device, const SphereSettings &settings)
{
    auto handle = ospNewGeometry("sphere");
    auto spheres = wrapObjectHandleAs<Spheres>(device, handle);

    setInterleavedSpheresParams(handle, settings.positionsAndRadii);
    setObjectDataIfNotEmpty(handle, "sphere.texcoord", settings.uvs);
    setObjectParam(handle, "type", OSP_SPHERE);

    commitObject(handle);

    return spheres;
}

Discs createDiscs(Device &device, const DiscSettings &settings)
{
    auto handle = ospNewGeometry("sphere");
    auto discs = wrapObjectHandleAs<Discs>(device, handle);

    setInterleavedSpheresParams(handle, settings.positionsAndRadii);
    setObjectDataIfNotEmpty(handle, "sphere.texcoord", settings.uvs);

    if (settings.normals.empty())
    {
        setObjectParam(handle, "type", OSP_DISC);
    }
    else
    {
        setObjectParam(handle, "type", OSP_ORIENTED_DISC);
        setObjectData(handle, "sphere.normal", settings.normals);
    }

    commitObject(handle);

    return discs;
}

Cylinders createCylinders(Device &device, const CylinderSettings &settings)
{
    auto handle = ospNewGeometry("curve");
    auto cylinders = wrapObjectHandleAs<Cylinders>(device, handle);

    setObjectData(handle, "vertex.position_radius", settings.positionsAndRadii);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", settings.uvs);
    setObjectDataIfNotEmpty(handle, "vertex.color", settings.colors);
    setObjectData(handle, "indices", settings.indices);

    setObjectParam(handle, "type", OSP_DISJOINT);
    setObjectParam(handle, "basis", OSP_LINEAR);

    commitObject(handle);

    return cylinders;
}

Curve createCurve(Device &device, const CurveSettings &settings)
{
    if (std::holds_alternative<RibbonCurve>(settings.type) && std::holds_alternative<LinearCurve>(settings.basis))
    {
        throw std::invalid_argument("OSPRay does not support ribbon curve with linear basis");
    }

    auto handle = ospNewGeometry("curve");
    auto curve = wrapObjectHandleAs<Curve>(device, handle);

    setObjectData(handle, "vertex.position_radius", settings.positionsAndRadii);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", settings.uvs);
    setObjectDataIfNotEmpty(handle, "vertex.color", settings.colors);
    setObjectData(handle, "indices", settings.indices);

    std::visit([=](const auto &value) { setCurveType(handle, value); }, settings.type);
    std::visit([=](const auto &value) { setCurveBasis(handle, value); }, settings.basis);

    commitObject(handle);

    return curve;
}

Boxes createBoxes(Device &device, const BoxSettings &settings)
{
    auto handle = ospNewGeometry("box");
    auto boxes = wrapObjectHandleAs<Boxes>(device, handle);

    setObjectData(handle, "box", settings.boxes);

    commitObject(handle);

    return boxes;
}

Planes createPlanes(Device &device, const PlaneSettings &settings)
{
    auto handle = ospNewGeometry("plane");
    auto planes = wrapObjectHandleAs<Planes>(device, handle);

    setObjectData(handle, "plane.coefficients", settings.coefficients);
    setObjectDataIfNotEmpty(handle, "plane.bounds", settings.bounds);

    commitObject(handle);

    return planes;
}

Isosurfaces createIsosurfaces(Device &device, const IsosurfaceSettings &settings)
{
    auto handle = ospNewGeometry("isosurface");
    auto isosurfaces = wrapObjectHandleAs<Isosurfaces>(device, handle);

    setObjectParam(handle, "volume", settings.volume);
    setObjectData(handle, "isovalue", settings.isovalues);

    commitObject(handle);

    return isosurfaces;
}
}

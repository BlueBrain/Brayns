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
    setObjectParam(handle, "vertex.position", settings.positions);
    setObjectParam(handle, "vertex.normal", settings.normals);
    setObjectParam(handle, "vertex.color", settings.colors);
    setObjectParam(handle, "vertex.texcoord", settings.uvs);
}

// TODO remove when OSPRay exposes sphere.position_radius to match embree internal layout
void setInterleavedSpheresParams(OSPGeometry handle, const std::vector<Vector4> &spheres)
{
    auto data = spheres.data();
    auto itemCount = Size3(spheres.size(), 1, 1);
    auto stride = Stride3(static_cast<std::ptrdiff_t>(sizeof(Vector4)), 0, 0);

    auto positionData = createData(data, OSP_VEC3F, itemCount, stride);
    setObjectParam(handle, "sphere.position", positionData);

    auto offset = sizeof(Vector3);

    auto radiusData = createData(data + offset, OSP_FLOAT, itemCount, stride);
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
    setObjectParam(handle, "vertex.normal", curve.normals);
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
    setObjectParam(handle, "vertex.tangent", curve.tangents);
}

void setCurveBasis(OSPGeometry handle, CatmullRomCurve)
{
    setObjectParam(handle, "basis", OSP_CATMULL_ROM);
}
}

namespace brayns
{
TriangleMesh createTriangleMesh(Device &device, const MeshSettings &settings, const std::vector<Index3> &indices)
{
    auto handle = ospNewGeometry("mesh");
    auto mesh = wrapObjectHandleAs<TriangleMesh>(device, handle);

    setMeshParams(handle, settings);

    setObjectParam(handle, "index", indices);

    commitObject(device, handle);

    return mesh;
}

QuadMesh createQuadMesh(Device &device, const MeshSettings &settings, const std::vector<Index4> &indices)
{
    auto handle = ospNewGeometry("mesh");
    auto mesh = wrapObjectHandleAs<QuadMesh>(device, handle);

    setMeshParams(handle, settings);

    setObjectParam(handle, "index", indices);

    if (indices.empty())
    {
        setObjectParam(handle, "quadSoup", true);
    }

    commitObject(device, handle);

    return mesh;
}

Spheres createSpheres(Device &device, const SphereSettings &settings)
{
    auto handle = ospNewGeometry("sphere");
    auto spheres = wrapObjectHandleAs<Spheres>(device, handle);

    setInterleavedSpheresParams(handle, settings.spheres);
    setObjectParam(handle, "sphere.texcoord", settings.uvs);
    setObjectParam(handle, "type", OSP_SPHERE);

    commitObject(device, handle);

    return spheres;
}

Discs createDiscs(Device &device, const DiscSettings &settings)
{
    auto handle = ospNewGeometry("sphere");
    auto discs = wrapObjectHandleAs<Discs>(device, handle);

    setInterleavedSpheresParams(handle, settings.spheres);
    setObjectParam(handle, "sphere.texcoord", settings.uvs);

    setObjectParam(handle, "sphere.normal", settings.normals);

    if (settings.normals.empty())
    {
        setObjectParam(handle, "type", OSP_DISC);
    }
    else
    {
        setObjectParam(handle, "type", OSP_ORIENTED_DISC);
    }

    commitObject(device, handle);

    return discs;
}

Cylinders createCylinders(Device &device, const CylinderSettings &settings)
{
    auto handle = ospNewGeometry("curve");
    auto cylinders = wrapObjectHandleAs<Cylinders>(device, handle);

    setObjectParam(handle, "vertex.position_radius", settings.spheres);
    setObjectParam(handle, "vertex.texcoord", settings.uvs);
    setObjectParam(handle, "vertex.color", settings.colors);
    setObjectParam(handle, "index", settings.indices);

    setObjectParam(handle, "type", OSP_DISJOINT);
    setObjectParam(handle, "basis", OSP_LINEAR);

    commitObject(device, handle);

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

    setObjectParam(handle, "vertex.position_radius", settings.spheres);
    setObjectParam(handle, "vertex.texcoord", settings.uvs);
    setObjectParam(handle, "vertex.color", settings.colors);
    setObjectParam(handle, "index", settings.indices);

    std::visit([=](const auto &value) { setCurveType(handle, value); }, settings.type);
    std::visit([=](const auto &value) { setCurveBasis(handle, value); }, settings.basis);

    commitObject(device, handle);

    return curve;
}

Boxes createBoxes(Device &device, const BoxSettings &settings)
{
    auto handle = ospNewGeometry("box");
    auto boxes = wrapObjectHandleAs<Boxes>(device, handle);

    setObjectParam(handle, "box", settings.boxes);

    commitObject(device, handle);

    return boxes;
}

Planes createPlanes(Device &device, const PlaneSettings &settings)
{
    auto handle = ospNewGeometry("plane");
    auto planes = wrapObjectHandleAs<Planes>(device, handle);

    setObjectParam(handle, "plane.coefficients", settings.coefficients);
    setObjectParam(handle, "plane.bounds", settings.bounds);

    commitObject(device, handle);

    return planes;
}

Isosurfaces createIsosurfaces(Device &device, const IsosurfaceSettings &settings)
{
    auto handle = ospNewGeometry("isosurface");
    auto isosurfaces = wrapObjectHandleAs<Isosurfaces>(device, handle);

    setObjectParam(handle, "volume", settings.volume);
    setObjectParam(handle, "isovalue", settings.isovalues);

    commitObject(device, handle);

    return isosurfaces;
}
}

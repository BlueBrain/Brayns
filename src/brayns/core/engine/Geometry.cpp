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
    auto data = reinterpret_cast<const char *>(spheres.data());
    auto itemCount = Size3(spheres.size(), 1, 1);
    auto stride = Stride3(static_cast<std::ptrdiff_t>(sizeof(Vector4)), 0, 0);
    auto radiusOffset = sizeof(Vector3);

    auto positionData = createData(data, OSP_VEC3F, itemCount, stride);
    setObjectParam(handle, "sphere.position", positionData);

    auto radiusData = createData(data + radiusOffset, OSP_FLOAT, itemCount, stride);
    setObjectParam(handle, "sphere.radius", radiusData);
}

void setSphereParams(OSPGeometry handle, const SphereSettings &settings)
{
    setInterleavedSpheresParams(handle, settings.positionsRadii);
    setObjectParam(handle, "sphere.texcoord", settings.uvs);
}

void setCurveBasis(OSPGeometry handle, LinearBasis)
{
    setObjectParam(handle, "basis", OSP_LINEAR);
}

void setCurveBasis(OSPGeometry handle, BezierBasis)
{
    setObjectParam(handle, "basis", OSP_BEZIER);
}

void setCurveBasis(OSPGeometry handle, BsplineBasis)
{
    setObjectParam(handle, "basis", OSP_BSPLINE);
}

void setCurveBasis(OSPGeometry handle, const HermiteBasis &curve)
{
    setObjectParam(handle, "basis", OSP_HERMITE);
    setObjectParam(handle, "vertex.tangent", curve.tangents);
}

void setCurveBasis(OSPGeometry handle, CatmullRomBasis)
{
    setObjectParam(handle, "basis", OSP_CATMULL_ROM);
}

void setCurveParams(OSPGeometry handle, const CurveSettings &settings)
{
    setObjectParam(handle, "vertex.position_radius", settings.positionsRadii);
    setObjectParam(handle, "vertex.texcoord", settings.uvs);
    setObjectParam(handle, "vertex.color", settings.colors);
    setObjectParam(handle, "index", settings.indices);
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

    setSphereParams(handle, settings);

    setObjectParam(handle, "type", OSP_SPHERE);

    commitObject(device, handle);

    return spheres;
}

Discs createDiscs(Device &device, const SphereSettings &settings, const std::vector<Vector3> &normals)
{
    auto handle = ospNewGeometry("sphere");
    auto discs = wrapObjectHandleAs<Discs>(device, handle);

    setSphereParams(handle, settings);

    setObjectParam(handle, "sphere.normal", normals);

    if (normals.empty())
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

Cylinders createCylinders(Device &device, const CurveSettings &settings)
{
    auto handle = ospNewGeometry("curve");
    auto cylinders = wrapObjectHandleAs<Cylinders>(device, handle);

    setCurveParams(handle, settings);

    setObjectParam(handle, "type", OSP_DISJOINT);
    setObjectParam(handle, "basis", OSP_LINEAR);

    commitObject(device, handle);

    return cylinders;
}

Curve createCurve(Device &device, const CurveSettings &settings, CurveType type, const CurveBasis &basis)
{
    auto handle = ospNewGeometry("curve");
    auto curve = wrapObjectHandleAs<Curve>(device, handle);

    setCurveParams(handle, settings);

    setObjectParam(handle, "type", static_cast<OSPCurveType>(type));

    std::visit([&](const auto &value) { setCurveBasis(handle, value); }, basis);

    commitObject(device, handle);

    return curve;
}

Ribbon createRibbon(Device &device, const CurveSettings &settings, const std::vector<Vector3> &normals, const RibbonBasis &basis)
{
    auto handle = ospNewGeometry("curve");
    auto curve = wrapObjectHandleAs<Ribbon>(device, handle);

    setCurveParams(handle, settings);

    setObjectParam(handle, "type", OSP_RIBBON);
    setObjectParam(handle, "vertex.normal", normals);

    std::visit([&](const auto &value) { setCurveBasis(handle, value); }, basis);

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

Isosurfaces createIsosurfaces(Device &device, const Volume &volume, const IsosurfaceSettings &settings)
{
    auto handle = ospNewGeometry("isosurface");
    auto isosurfaces = wrapObjectHandleAs<Isosurfaces>(device, handle);

    setObjectParam(handle, "volume", volume);
    setObjectParam(handle, "isovalue", settings.isovalues);

    commitObject(device, handle);

    return isosurfaces;
}
}

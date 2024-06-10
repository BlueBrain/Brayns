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
using brayns::Vector3;
using namespace brayns::experimental;

void setMeshVerticesParams(OSPGeometry handle, const MeshVertices &vertices)
{
    setObjectData(handle, "vertex.position", vertices.positions);
    setObjectDataIfNotEmpty(handle, "vertex.normal", vertices.normals);
    setObjectDataIfNotEmpty(handle, "vertex.color", vertices.colors);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", vertices.uvs);
}

// TODO remove when OSPRay exposes sphere.position_radius to match embree internal layout
void setInterleavedSpheresParams(OSPGeometry handle, std::span<PositionRadius> points)
{
    constexpr auto stride = sizeof(PositionRadius);

    auto size = points.size();

    const auto *positions = points.data();
    const auto *radii = &positions[0].w;

    auto positionData = Data(ospNewSharedData(positions, OSP_VEC3F, size, stride));
    setObjectParam(handle, "sphere.position", positionData);

    auto radiusData = Data(ospNewSharedData(radii, OSP_FLOAT, size, stride));
    setObjectParam(handle, "sphere.radius", radiusData);
}

void setCurveVerticesParams(OSPGeometry handle, const CurveVertices &vertices)
{
    setObjectData(handle, "vertex.position_radius", vertices.points);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", vertices.uvs);
    setObjectDataIfNotEmpty(handle, "vertex.color", vertices.colors);
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

namespace brayns::experimental
{
void Mesh::setColors(std::span<Color4> colors)
{
    auto handle = getHandle();
    setObjectDataIfNotEmpty(handle, "vertex.color", colors);
    commitObject(handle);
}

OSPGeometry ObjectReflector<TriangleMesh>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("mesh");
    throwLastDeviceErrorIfNull(device, handle);

    setMeshVerticesParams(handle, settings.vertices);
    setObjectDataIfNotEmpty(handle, "index", settings.indices);

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<QuadMesh>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("mesh");
    throwLastDeviceErrorIfNull(device, handle);

    setMeshVerticesParams(handle, settings.vertices);

    if (settings.indices.empty())
    {
        setObjectParam(handle, "quadSoup", true);
    }
    else
    {
        setObjectData(handle, "index", settings.indices);
    }

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<Spheres>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("sphere");
    throwLastDeviceErrorIfNull(device, handle);

    setInterleavedSpheresParams(handle, settings.points);
    setObjectDataIfNotEmpty(handle, "sphere.texcoord", settings.uvs);
    setObjectParam(handle, "type", OSP_SPHERE);

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<Discs>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("sphere");
    throwLastDeviceErrorIfNull(device, handle);

    setInterleavedSpheresParams(handle, settings.points);
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

    return handle;
}

void Cylinders::setColors(std::span<Color4> colors)
{
    auto handle = getHandle();
    setObjectDataIfNotEmpty(handle, "vertex.color", colors);
    commitObject(handle);
}

OSPGeometry ObjectReflector<Cylinders>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("curve");
    throwLastDeviceErrorIfNull(device, handle);

    setCurveVerticesParams(handle, settings.vertices);
    setObjectData(handle, "indices", settings.indices);

    setObjectParam(handle, "type", OSP_DISJOINT);
    setObjectParam(handle, "basis", OSP_LINEAR);

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<Curve>::createHandle(OSPDevice device, const Settings &settings)
{
    if (std::holds_alternative<RibbonCurve>(settings.type) && std::holds_alternative<LinearCurve>(settings.basis))
    {
        throw std::invalid_argument("OSPRay does not support ribbon curve with linear basis");
    }

    auto handle = ospNewGeometry("curve");
    throwLastDeviceErrorIfNull(device, handle);

    setCurveVerticesParams(handle, settings.vertices);
    setObjectData(handle, "indices", settings.indices);

    std::visit([=](const auto &value) { setCurveType(handle, value); }, settings.type);
    std::visit([=](const auto &value) { setCurveBasis(handle, value); }, settings.basis);

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<Boxes>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("box");
    throwLastDeviceErrorIfNull(device, handle);

    setObjectData(handle, "box", settings.boxes);

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<Planes>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("plane");
    throwLastDeviceErrorIfNull(device, handle);

    setObjectData(handle, "plane.coefficients", settings.coefficients);
    setObjectDataIfNotEmpty(handle, "plane.bounds", settings.bounds);

    commitObject(handle);

    return handle;
}

OSPGeometry ObjectReflector<Isosurfaces>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometry("isosurface");
    throwLastDeviceErrorIfNull(device, handle);

    setObjectParam(handle, "volume", settings.volume);
    setObjectData(handle, "isovalue", settings.isovalues);

    commitObject(handle);

    return handle;
}
}

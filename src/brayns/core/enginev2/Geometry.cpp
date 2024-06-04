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
using namespace brayns::experimental;

void setMeshParams(OSPGeometry handle, const MeshSettings &settings)
{
    setObjectParam(handle, "vertex.position", settings.positions);

    if (settings.normals)
    {
        setObjectParam(handle, "vertex.normal", *settings.normals);
    }

    if (settings.colors)
    {
        setObjectParam(handle, "vertex.color", *settings.colors);
    }

    if (settings.uvs)
    {
        setObjectParam(handle, "vertex.texcoord", *settings.uvs);
    }
}

void setSphereParams(OSPGeometry handle, const SphereSettings &settings)
{
    setObjectParam(handle, "sphere.position", settings.positions);

    std::visit([=](const auto &radius) { setObjectParam(handle, "sphere.radius", radius); }, settings.radii);

    if (settings.uvs)
    {
        setObjectParam(handle, "sphere.texcoord", *settings.uvs);
    }
}

void setCurveParams(OSPGeometry handle, const CylinderSettings &settings)
{
    setObjectParam(handle, "vertex.position_radius", settings.samples);
    setObjectParam(handle, "index", settings.indices);

    if (settings.uvs)
    {
        setObjectParam(handle, "vertex.texcoord", *settings.uvs);
    }

    if (settings.colors)
    {
        setObjectParam(handle, "vertex.color", *settings.colors);
    }
}
}

namespace brayns::experimental
{
void Mesh::setColors(const Data1D<Color4> &colors)
{
    auto handle = getHandle();
    setObjectParam(handle, "vertex.color", colors);
    commitObject(handle);
}

void loadMeshParams(OSPGeometry handle, const TriangleMeshSettings &settings)
{
    setMeshParams(handle, settings);

    if (settings.indices)
    {
        setObjectParam(handle, "index", *settings.indices);
    }

    commitObject(handle);
}

void loadMeshParams(OSPGeometry handle, const QuadMeshSettings &settings)
{
    setMeshParams(handle, settings);

    if (settings.indices)
    {
        setObjectParam(handle, "index", *settings.indices);
    }
    else
    {
        setObjectParam(handle, "quadSoup", true);
    }

    commitObject(handle);
}

void loadSphereParams(OSPGeometry handle, const SphereSettings &settings)
{
    setSphereParams(handle, settings);
    commitObject(handle);
}

void loadSphereParams(OSPGeometry handle, const DiscSettings &settings)
{
    setSphereParams(handle, settings);

    auto type = settings.normals ? OSP_ORIENTED_DISC : OSP_DISC;

    setObjectParam(handle, "type", type);

    if (settings.normals)
    {
        setObjectParam(handle, "sphere.normal", *settings.normals);
    }

    commitObject(handle);
}

void Spheres::setRadii(const Data1D<float> &radii)
{
    auto handle = getHandle();
    setObjectParam(handle, "sphere.radius", radii);
    commitObject(handle);
}

void Spheres::setRadius(float radius)
{
    auto handle = getHandle();
    setObjectParam(handle, "sphere.radius", radius);
    commitObject(handle);
}

void loadCurveParams(OSPGeometry handle, const CylinderSettings &settings)
{
    setCurveParams(handle, settings);
    commitObject(handle);
}

void loadCurveParams(OSPGeometry handle, const CurveSettings &settings)
{
    setCurveParams(handle, settings);
    setObjectParam(handle, "type", static_cast<OSPCurveType>(settings.type));
    setObjectParam(handle, "basis", static_cast<OSPCurveBasis>(settings.basis));
    commitObject(handle);
}

void loadCurveParams(OSPGeometry handle, const RibbonCurveSettings &settings)
{
    if (settings.basis == CurveBasis::Linear)
    {
        throw std::invalid_argument("Ribbon curve with linear basis not supported");
    }

    setCurveParams(handle, settings);

    setObjectParam(handle, "vertex.normal", settings.normals);
    setObjectParam(handle, "type", OSP_RIBBON);
    setObjectParam(handle, "basis", static_cast<OSPCurveBasis>(settings.basis));

    commitObject(handle);
}

void loadCurveParams(OSPGeometry handle, const HermiteCurveSettings &settings)
{
    setCurveParams(handle, settings);
    setObjectParam(handle, "vertex.tangent", settings.tangents);
    setObjectParam(handle, "type", static_cast<OSPCurveType>(settings.type));
    setObjectParam(handle, "basis", OSP_HERMITE);
    commitObject(handle);
}

void Curve::setSamples(const Data1D<PositionRadius> &samples)
{
    auto handle = getHandle();
    setObjectParam(handle, "vertex.position_radius", samples);
    commitObject(handle);
}

void Curve::setColors(const Data1D<Color4> &colors)
{
    auto handle = getHandle();
    setObjectParam(handle, "vertex.color", colors);
    commitObject(handle);
}

void loadBoxParams(OSPGeometry handle, const BoxSettings &settings)
{
    setObjectParam(handle, "box", settings.boxes);
    commitObject(handle);
}

void loadPlaneParams(OSPGeometry handle, const PlaneSettings &settings)
{
    setObjectParam(handle, "plane.coefficients", settings.coefficients);

    if (settings.bounds)
    {
        setObjectParam(handle, "plane.bounds", *settings.bounds);
    }

    commitObject(handle);
}

void loadIsosurfaceParams(OSPGeometry handle, const IsosurfaceSettings &settings)
{
    setObjectParam(handle, "volume", settings.volume);

    std::visit([=](const auto &isovalues) { setObjectParam(handle, "isovalue", isovalues); }, settings.isovalues);

    commitObject(handle);
}
}

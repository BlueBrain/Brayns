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

namespace
{
using namespace brayns::experimental;

void setMeshParams(OSPGeometry handle, const MeshSettings &settings)
{
    setObjectData(handle, "vertex.position", settings.positions);
    setObjectDataIfNotEmpty(handle, "vertex.normal", settings.normals);
    setObjectDataIfNotEmpty(handle, "vertex.color", settings.colors);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", settings.uvs);
}

void setSphereParams(OSPGeometry handle, const SphereSettings &settings)
{
    setObjectData(handle, "sphere.position", settings.positions);
    setObjectData(handle, "sphere.radius", settings.radii);
    setObjectDataIfNotEmpty(handle, "sphere.texcoord", settings.uvs);
}

void setCurveParams(OSPGeometry handle, const CylinderSettings &settings)
{
    setObjectData(handle, "vertex.position_radius", settings.samples);
    setObjectData(handle, "index", settings.indices);
    setObjectDataIfNotEmpty(handle, "vertex.texcoord", settings.uvs);
    setObjectDataIfNotEmpty(handle, "vertex.color", settings.colors);
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

void ObjectReflector<TriangleMesh>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setMeshParams(handle, settings);
    setObjectDataIfNotEmpty(handle, "index", settings.indices);
    commitObject(handle);
}

void ObjectReflector<QuadMesh>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setMeshParams(handle, settings);

    if (!settings.indices.empty())
    {
        setObjectData(handle, "index", settings.indices);
    }
    else
    {
        setObjectParam(handle, "quadSoup", true);
    }

    commitObject(handle);
}

void ObjectReflector<Spheres>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setSphereParams(handle, settings);
    commitObject(handle);
}

void ObjectReflector<Discs>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setSphereParams(handle, settings);

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
}

void ObjectReflector<Cylinders>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setCurveParams(handle, settings);
    commitObject(handle);
}

void Cylinders::setColors(std::span<Color4> colors)
{
    auto handle = getHandle();
    setObjectDataIfNotEmpty(handle, "vertex.color", colors);
    commitObject(handle);
}

void ObjectReflector<Curve>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setCurveParams(handle, settings);
    setObjectParam(handle, "type", static_cast<OSPCurveType>(settings.type));
    setObjectParam(handle, "basis", static_cast<OSPCurveBasis>(settings.basis));
    commitObject(handle);
}

void ObjectReflector<Ribbon>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setCurveParams(handle, settings);
    setObjectData(handle, "vertex.normal", settings.normals);
    setObjectParam(handle, "type", OSP_RIBBON);
    setObjectParam(handle, "basis", static_cast<OSPCurveBasis>(settings.basis));
    commitObject(handle);
}

void ObjectReflector<Boxes>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setObjectData(handle, "box", settings.boxes);
    commitObject(handle);
}

void ObjectReflector<Planes>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setObjectData(handle, "plane.coefficients", settings.coefficients);
    setObjectDataIfNotEmpty(handle, "plane.bounds", settings.bounds);
    commitObject(handle);
}

void ObjectReflector<Isosurfaces>::loadParams(OSPGeometry handle, const Settings &settings)
{
    setObjectParam(handle, "volume", settings.volume);
    setObjectData(handle, "isovalue", settings.isovalues);
    commitObject(handle);
}
}

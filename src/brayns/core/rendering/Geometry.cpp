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

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace brayns
{
void GeometricModel::setGeometry(const BaseGeometry &geometry)
{
    setParam("geometry", geometry.getHandle());
}

void GeometricModel::setMaterial(std::uint32_t rendererIndex)
{
    setParam("material", rendererIndex);
}

void GeometricModel::setPrimitiveMaterials(SharedArray<std::uint32_t> rendererIndices)
{
    setParam("material", toSharedData(rendererIndices));
}

void GeometricModel::setPrimitiveColors(SharedArray<Color4> colors)
{
    setParam("color", toSharedData(colors));
}

void GeometricModel::setColor(const Color4 &color)
{
    setParam("color", color);
}

void GeometricModel::removeColors()
{
    removeParam("color");
}

void GeometricModel::invertNormals(bool inverted)
{
    setParam("invertNormals", inverted);
}

void GeometricModel::setId(std::uint32_t id)
{
    setParam("id", id);
}

void MeshGeometry::setVertexPositions(SharedArray<Vector3> positions)
{
    setParam("vertex.position", toSharedData(positions));
}

void MeshGeometry::setVertexNormals(SharedArray<Vector3> normals)
{
    setParam("vertex.normal", toSharedData(normals));
}

void MeshGeometry::setVertexColors(SharedArray<Color4> colors)
{
    setParam("vertex.color", toSharedData(colors));
}

void MeshGeometry::setTriangleIndices(SharedArray<Index3> indices)
{
    setParam("index", toSharedData(indices));
}

void MeshGeometry::setQuadIndices(SharedArray<Index4> indices)
{
    setParam("index", toSharedData(indices));
}

void MeshGeometry::setQuadSoup(bool quadSoup)
{
    setParam("quadSoup", quadSoup);
}

void SphereGeometry::setPositions(SharedArray<Vector3> positions)
{
    setParam("sphere.position", toSharedData(positions));
}

void SphereGeometry::setRadii(SharedArray<float> radii)
{
    setParam("sphere.radius", toSharedData(radii));
}

void SphereGeometry::setRadius(float radius)
{
    setParam("radius", radius);
}

void CurveGeometry::setVertexPositionsAndRadii(SharedArray<PositionRadius> positionsRadii)
{
    setParam("vertex.position_radius", toSharedData(positionsRadii));
}

void CurveGeometry::setVertexColors(SharedArray<Color4> colors)
{
    setParam("vertex.color", toSharedData(colors));
}

void CurveGeometry::setIndices(SharedArray<std::uint32_t> indices)
{
    setParam("index", toSharedData(indices));
}

void CurveGeometry::setType(CurveType type)
{
    setParam("type", static_cast<OSPCurveType>(type));
}

void CurveGeometry::setBasis(CurveBasis basis)
{
    setParam("basis", static_cast<OSPCurveBasis>(basis));
}

void BoxGeometry::setBoxes(SharedArray<Box3> boxes)
{
    setParam("box", toSharedData(boxes));
}

void PlaneGeometry::setCoefficients(SharedArray<Vector4> coefficients)
{
    setParam("plane.coefficients", toSharedData(coefficients));
}

void PlaneGeometry::setBounds(SharedArray<Box3> bounds)
{
    setParam("plane.bounds", toSharedData(bounds));
}

void IsosurfaceGeometry::setVolume(const BaseVolume &volume)
{
    setParam("volume", volume.getHandle());
}

void IsosurfaceGeometry::setIsovalues(SharedArray<float> values)
{
    setParam("isovalue", toSharedData(values));
}

void IsosurfaceGeometry::setIsovalue(float value)
{
    setParam("isovalue", value);
}
}

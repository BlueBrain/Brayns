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

namespace brayns::experimental
{
void GeometricModel::setGeometry(const Geometry &geometry)
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

void Mesh::setVertexPositions(SharedArray<Vector3> positions)
{
    setParam("vertex.position", toSharedData(positions));
}

void Mesh::setVertexNormals(SharedArray<Vector3> normals)
{
    setParam("vertex.normal", toSharedData(normals));
}

void Mesh::setVertexColors(SharedArray<Color4> colors)
{
    setParam("vertex.color", toSharedData(colors));
}

void Mesh::setTriangleIndices(SharedArray<Index3> indices)
{
    setParam("index", toSharedData(indices));
}

void Mesh::setQuadIndices(SharedArray<Index4> indices)
{
    setParam("index", toSharedData(indices));
}

void Mesh::setQuadSoup(bool quadSoup)
{
    setParam("quadSoup", quadSoup);
}

void Spheres::setPositions(SharedArray<Vector3> positions)
{
    setParam("sphere.position", toSharedData(positions));
}

void Spheres::setRadii(SharedArray<float> radii)
{
    setParam("sphere.radius", toSharedData(radii));
}

void Spheres::setRadius(float radius)
{
    setParam("radius", radius);
}

void Curve::setVertexPositionsAndRadii(SharedArray<PositionRadius> positionsRadii)
{
    setParam("vertex.position_radius", toSharedData(positionsRadii));
}

void Curve::setVertexColors(SharedArray<Color4> colors)
{
    setParam("vertex.color", toSharedData(colors));
}

void Curve::setIndices(SharedArray<std::uint32_t> indices)
{
    setParam("index", toSharedData(indices));
}

void Curve::setType(CurveType type)
{
    setParam("type", static_cast<OSPCurveType>(type));
}

void Curve::setBasis(CurveBasis basis)
{
    setParam("basis", static_cast<OSPCurveBasis>(basis));
}

void Boxes::setBoxes(SharedArray<Box3> boxes)
{
    setParam("box", toSharedData(boxes));
}

void Planes::setCoefficients(SharedArray<Vector4> coefficients)
{
    setParam("plane.coefficients", toSharedData(coefficients));
}

void Planes::setBounds(SharedArray<Box3> bounds)
{
    setParam("plane.bounds", toSharedData(bounds));
}

void Isosurfaces::setVolume(const Volume &volume)
{
    setParam("volume", volume.getHandle());
}

void Isosurfaces::setIsovalues(SharedArray<float> values)
{
    setParam("isovalue", toSharedData(values));
}

void Isosurfaces::setIsovalue(float value)
{
    setParam("isovalue", value);
}
}

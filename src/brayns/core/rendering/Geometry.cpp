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
void GeometryModel::setGeometry(const ospray::cpp::Geometry &geometry)
{
    setParam("geometry", geometry);
}

void GeometryModel::setMaterial(std::uint32_t rendererIndex)
{
    setParam("material", rendererIndex);
}

void GeometryModel::setPrimitiveMaterials(const std::vector<std::uint32_t> &rendererIndices)
{
    setParam("material", ospray::cpp::SharedData(rendererIndices));
}

void GeometryModel::setPrimitiveColors(const std::vector<Color4> &colors)
{
    setParam("color", ospray::cpp::SharedData(colors));
}

void GeometryModel::setColor(const Color4 &color)
{
    setParam("color", color);
}

void GeometryModel::removeColors()
{
    removeParam("color");
}

void GeometryModel::invertNormals(bool inverted)
{
    setParam("invertNormals", inverted);
}

void GeometryModel::setId(std::uint32_t id)
{
    setParam("id", id);
}

void MeshGeometry::setVertexPositions(const std::vector<Vector3> &positions)
{
    setParam("vertex.position", ospray::cpp::SharedData(positions));
}

void MeshGeometry::setVertexNormals(const std::vector<Vector3> &normals)
{
    setParam("vertex.normal", ospray::cpp::SharedData(normals));
}

void MeshGeometry::setVertexColors(const std::vector<Color4> &colors)
{
    setParam("vertex.color", ospray::cpp::SharedData(colors));
}

void MeshGeometry::setTriangleIndices(const std::vector<Index3> &indices)
{
    setParam("index", ospray::cpp::SharedData(indices));
}

void MeshGeometry::setQuadIndices(const std::vector<Index4> &indices)
{
    setParam("index", ospray::cpp::SharedData(indices));
}

void MeshGeometry::setQuadSoup(bool quadSoup)
{
    setParam("quadSoup", quadSoup);
}

void SphereGeometry::setPositions(const std::vector<Vector3> &positions)
{
    setParam("sphere.position", ospray::cpp::SharedData(positions));
}

void SphereGeometry::setRadii(const std::vector<float> radii)
{
    setParam("sphere.radius", ospray::cpp::SharedData(radii));
}

void SphereGeometry::setRadius(float radius)
{
    setParam("radius", radius);
}

void CurveGeometry::setVertexPositionsAndRadii(const std::vector<PositionRadius> &positionsRadii)
{
    setParam("vertex.position_radius", ospray::cpp::SharedData(positionsRadii));
}

void CurveGeometry::setVertexColors(const std::vector<Color4> &colors)
{
    setParam("vertex.color", ospray::cpp::SharedData(colors));
}

void CurveGeometry::setIndices(const std::vector<std::uint32_t> &indices)
{
    setParam("index", ospray::cpp::SharedData(indices));
}

void CurveGeometry::setType(CurveType type)
{
    setParam("type", static_cast<OSPCurveType>(type));
}

void CurveGeometry::setBasis(CurveBasis basis)
{
    setParam("basis", static_cast<OSPCurveBasis>(basis));
}

void BoxGeometry::setBoxes(const std::vector<Box3> &boxes)
{
    setParam("box", ospray::cpp::SharedData(boxes));
}

void PlaneGeometry::setCoefficients(const std::vector<Vector4> &coefficients)
{
    setParam("plane.coefficients", ospray::cpp::SharedData(coefficients));
}

void PlaneGeometry::setBounds(const std::vector<Box3> &bounds)
{
    setParam("plane.bounds", ospray::cpp::SharedData(bounds));
}

void IsosurfaceGeometry::setVolume(const ospray::cpp::Volume &volume)
{
    setParam("volume", volume);
}

void IsosurfaceGeometry::setIsovalues(const std::vector<float> &values)
{
    setParam("isovalue", ospray::cpp::SharedData(values));
}

void IsosurfaceGeometry::setIsovalue(float value)
{
    setParam("isovalue", value);
}
}

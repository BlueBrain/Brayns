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

namespace
{
const std::string colorKey = "color";
const std::string idKey = "id";
const std::string invertNormalsKey = "invertNormals";

const std::string vertexPositionKey = "vertex.position";
const std::string vertexNormalKey = "vertex.normal";
const std::string vertexColorKey = "vertex.color";
const std::string quadSoupKey = "quadSoup";

const std::string spherePositionKey = "sphere.position";
const std::string sphereRadiusKey = "sphere.radius";
const std::string radiusKey = "radius";

const std::string vertexPositionRadiusKey = "vertex.position_radius";
const std::string indexKey = "index";
const std::string typeKey = "type";
const std::string basisKey = "basis";

const std::string boxKey = "box";

const std::string planeCoefficientsKey = "plane.coefficients";
const std::string planeBoundsKey = "plane.bounds";

const std::string volumeKey = "volume";
const std::string isovalueKey = "isovalue";
}

namespace brayns
{
GeometryModel::GeometryModel(ospray::cpp::GeometricModel model):
    _model(std::move(model))
{
}

ospray::cpp::GeometricModel GeometryModel::getHandle() const
{
    return _model;
}

void GeometryModel::setId(std::uint32_t id)
{
    _model.setParam(idKey, id);
}

void GeometryModel::invertNormals(bool inverted)
{
    _model.setParam(invertNormalsKey, inverted);
}

void GeometryModel::setPrimitiveColors(const std::vector<Color4> &colors)
{
    _model.setParam(colorKey, ospray::cpp::SharedData(colors));
}

void GeometryModel::setColor(const Color4 &color)
{
    _model.setParam(colorKey, color);
}

void GeometryModel::removeColors()
{
    _model.removeParam(colorKey);
}

void GeometryModel::commit()
{
    _model.commit();
}

MeshGeometry::MeshGeometry(ospray::cpp::Geometry geometry):
    _geometry(std::move(geometry))
{
}

ospray::cpp::Geometry MeshGeometry::getHandle() const
{
    return _geometry;
}

void MeshGeometry::setVertexPositions(const std::vector<Vector3> &positions)
{
    _geometry.setParam(vertexPositionKey, ospray::cpp::SharedData(positions));
}

void MeshGeometry::setVertexNormals(const std::vector<Vector3> &normals)
{
    _geometry.setParam(vertexNormalKey, ospray::cpp::SharedData(normals));
}

void MeshGeometry::setVertexColors(const std::vector<Color4> &colors)
{
    _geometry.setParam(vertexColorKey, ospray::cpp::SharedData(colors));
}

void MeshGeometry::setTriangleIndices(const std::vector<Index3> &indices)
{
    _geometry.setParam(indexKey, ospray::cpp::SharedData(indices));
}

void MeshGeometry::setQuadIndices(const std::vector<Index4> &indices)
{
    _geometry.setParam(indexKey, ospray::cpp::SharedData(indices));
}

void MeshGeometry::setQuadSoup(bool quadSoup)
{
    _geometry.setParam(quadSoupKey, quadSoup);
}

void MeshGeometry::commit()
{
    _geometry.commit();
}

SphereGeometry::SphereGeometry(ospray::cpp::Geometry geometry):
    _geometry(std::move(geometry))
{
}

ospray::cpp::Geometry SphereGeometry::getHandle() const
{
    return _geometry;
}

void SphereGeometry::setPositions(const std::vector<Vector3> &positions)
{
    _geometry.setParam(spherePositionKey, ospray::cpp::SharedData(positions));
}

void SphereGeometry::setRadii(const std::vector<float> radii)
{
    _geometry.setParam(sphereRadiusKey, ospray::cpp::SharedData(radii));
}

void SphereGeometry::setRadius(float radius)
{
    _geometry.setParam(radiusKey, radius);
}

void SphereGeometry::commit()
{
    _geometry.commit();
}

CurveGeometry::CurveGeometry(ospray::cpp::Geometry geometry):
    _geometry(std::move(geometry))
{
}

ospray::cpp::Geometry CurveGeometry::getHandle() const
{
    return _geometry;
}

void CurveGeometry::setVertexPositionsAndRadii(const std::vector<PositionRadius> &positionsRadii)
{
    _geometry.setParam(vertexPositionRadiusKey, ospray::cpp::SharedData(positionsRadii));
}

void CurveGeometry::setVertexColors(const std::vector<Color4> &colors)
{
    _geometry.setParam(vertexColorKey, ospray::cpp::SharedData(colors));
}

void CurveGeometry::setIndices(const std::vector<std::uint32_t> &indices)
{
    _geometry.setParam(indexKey, ospray::cpp::SharedData(indices));
}

void CurveGeometry::setType(CurveType type)
{
    _geometry.setParam(typeKey, static_cast<unsigned int>(type));
}

void CurveGeometry::setBasis(CurveBasis basis)
{
    _geometry.setParam(basisKey, static_cast<unsigned int>(basis));
}

void CurveGeometry::commit()
{
    _geometry.commit();
}

BoxGeometry::BoxGeometry(ospray::cpp::Geometry geometry):
    _geometry(std::move(geometry))
{
}

ospray::cpp::Geometry BoxGeometry::getHandle() const
{
    return _geometry;
}

void BoxGeometry::setBoxes(const std::vector<Box3> &boxes)
{
    _geometry.setParam(boxKey, ospray::cpp::SharedData(boxes));
}

void BoxGeometry::commit()
{
    _geometry.commit();
}

PlaneGeometry::PlaneGeometry(ospray::cpp::Geometry geometry):
    _geometry(std::move(geometry))
{
}

ospray::cpp::Geometry PlaneGeometry::getHandle() const
{
    return _geometry;
}

void PlaneGeometry::setCoefficients(const std::vector<Vector4> &coefficients)
{
    _geometry.setParam(planeCoefficientsKey, ospray::cpp::SharedData(coefficients));
}

void PlaneGeometry::setBounds(const std::vector<Box3> &bounds)
{
    _geometry.setParam(planeBoundsKey, ospray::cpp::SharedData(bounds));
}

void PlaneGeometry::commit()
{
    _geometry.commit();
}

IsosurfaceGeometry::IsosurfaceGeometry(ospray::cpp::Geometry geometry):
    _geometry(std::move(geometry))
{
}

ospray::cpp::Geometry IsosurfaceGeometry::getHandle() const
{
    return _geometry;
}

void IsosurfaceGeometry::setVolume(ospray::cpp::Volume volume)
{
    _geometry.setParam(volumeKey, volume);
}

void IsosurfaceGeometry::setIsovalues(const std::vector<float> &values)
{
    _geometry.setParam(isovalueKey, ospray::cpp::SharedData(values));
}

void IsosurfaceGeometry::setIsovalue(float value)
{
    _geometry.setParam(isovalueKey, value);
}

void IsosurfaceGeometry::commit()
{
    _geometry.commit();
}
}

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

#pragma once

#include <cassert>
#include <typeindex>
#include <vector>

#include <ospray/ospray_cpp.h>

#include <brayns/core/utils/Math.h>

namespace brayns
{
class GeometryModel
{
public:
    explicit GeometryModel(ospray::cpp::GeometricModel model);

    ospray::cpp::GeometricModel getHandle() const;
    void setId(std::uint32_t id);
    void invertNormals(bool inverted);
    void setPerPrimitiveColors(const std::vector<Color4> &colors);
    void setColor(const Color4 &color);
    void removeColors();
    void commit();

private:
    ospray::cpp::GeometricModel _model;
};

class MeshGeometry
{
public:
    explicit MeshGeometry(ospray::cpp::Geometry geometry);

    ospray::cpp::Geometry getHandle() const;
    void setPositions(const std::vector<Vector3> &positions);
    void setNormals(const std::vector<Vector3> &normals);
    void setColors(const std::vector<Color4> &colors);
    void setTriangleIndices(const std::vector<Vector3ui> &indices);
    void setQuadIndices(const std::vector<Vector4ui> &indices);
    void setQuadSoup(bool quadSoup);
    void commit();

private:
    ospray::cpp::Geometry _geometry;
};

class SphereGeometry
{
public:
    explicit SphereGeometry(ospray::cpp::Geometry geometry);

    ospray::cpp::Geometry getHandle() const;
    void setPositions(const std::vector<Vector3> &positions);
    void setRadii(const std::vector<float> radii);
    void setRadius(float radius);
    void commit();

private:
    ospray::cpp::Geometry _geometry;
};

using PositionRadius = Vector4;

enum class CurveType
{
    Flat = OSP_FLAT,
    Round = OSP_ROUND,
    Disjoint = OSP_DISJOINT,
};

enum class CurveBasis
{
    Linear = OSP_LINEAR,
    Bezier = OSP_BEZIER,
    BSpline = OSP_BSPLINE,
};

class CurveGeometry
{
public:
    explicit CurveGeometry(ospray::cpp::Geometry geometry);

    ospray::cpp::Geometry getHandle() const;
    void setControlPoints(const std::vector<PositionRadius> &controlPoints);
    void setPerControlPointColors(const std::vector<Color4> &colors);
    void setIndices(const std::vector<std::uint32_t> &indices);
    void setType(CurveType type);
    void setBasis(CurveBasis basis);
    void commit();

private:
    ospray::cpp::Geometry _geometry;
};

class BoxGeometry
{
public:
    explicit BoxGeometry(ospray::cpp::Geometry geometry);

    ospray::cpp::Geometry getHandle() const;
    void setBoxes(const std::vector<Box3> &boxes);
    void commit();

private:
    ospray::cpp::Geometry _geometry;
};

class PlaneGeometry
{
public:
    explicit PlaneGeometry(ospray::cpp::Geometry geometry);

    ospray::cpp::Geometry getHandle() const;
    void setCoefficients(const std::vector<Vector4> &coefficients);
    void setBounds(const std::vector<Box3> &bounds);
    void commit();

private:
    ospray::cpp::Geometry _geometry;
};

class IsosurfaceGeometry
{
public:
    explicit IsosurfaceGeometry(ospray::cpp::Geometry geometry);

    ospray::cpp::Geometry getHandle() const;
    void setIsovalues(const std::vector<float> &values);
    void setIsovalue(float value);
    void commit();

private:
    ospray::cpp::Geometry _geometry;
};
}

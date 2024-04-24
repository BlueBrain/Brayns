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

#include <vector>

#include <ospray/ospray_cpp.h>

#include <brayns/core/utils/Math.h>

#include "Object.h"

namespace brayns
{
class BaseGeometry : public Object<ospray::cpp::Geometry>
{
public:
    using Object::Object;
};

class GeometryModel : public Object<ospray::cpp::GeometricModel>
{
public:
    using Object::Object;

    void setGeometry(const ospray::cpp::Geometry &geometry);
    void setMaterial(std::uint32_t rendererIndex);
    void setPrimitiveMaterials(const std::vector<std::uint32_t> &rendererIndices);
    void setPrimitiveColors(const std::vector<Color4> &colors);
    void setColor(const Color4 &color);
    void removeColors();
    void invertNormals(bool inverted);
    void setId(std::uint32_t id);
};

class MeshGeometry : public BaseGeometry
{
public:
    using BaseGeometry::BaseGeometry;

    static inline const std::string name = "mesh";

    void setVertexPositions(const std::vector<Vector3> &positions);
    void setVertexNormals(const std::vector<Vector3> &normals);
    void setVertexColors(const std::vector<Color4> &colors);
    void setTriangleIndices(const std::vector<Index3> &indices);
    void setQuadIndices(const std::vector<Index4> &indices);
    void setQuadSoup(bool quadSoup);
};

class SphereGeometry : public BaseGeometry
{
public:
    using BaseGeometry::BaseGeometry;

    static inline const std::string name = "sphere";

    void setPositions(const std::vector<Vector3> &positions);
    void setRadii(const std::vector<float> radii);
    void setRadius(float radius);
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

class CurveGeometry : public BaseGeometry
{
public:
    using BaseGeometry::BaseGeometry;

    static inline const std::string name = "curve";

    void setVertexPositionsAndRadii(const std::vector<PositionRadius> &positionsRadii);
    void setVertexColors(const std::vector<Color4> &colors);
    void setIndices(const std::vector<std::uint32_t> &indices);
    void setType(CurveType type);
    void setBasis(CurveBasis basis);
};

class BoxGeometry : public BaseGeometry
{
public:
    using BaseGeometry::BaseGeometry;

    static inline const std::string name = "box";

    void setBoxes(const std::vector<Box3> &boxes);
};

class PlaneGeometry : public BaseGeometry
{
public:
    using BaseGeometry::BaseGeometry;

    static inline const std::string name = "plane";

    void setCoefficients(const std::vector<Vector4> &coefficients);
    void setBounds(const std::vector<Box3> &bounds);
};

class IsosurfaceGeometry : public BaseGeometry
{
public:
    using BaseGeometry::BaseGeometry;

    static inline const std::string name = "isosurface";

    void setVolume(const ospray::cpp::Volume &volume);
    void setIsovalues(const std::vector<float> &values);
    void setIsovalue(float value);
};
}

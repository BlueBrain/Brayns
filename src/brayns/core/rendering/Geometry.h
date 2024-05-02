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

#include "Data.h"
#include "Managed.h"
#include "Volume.h"

namespace brayns::experimental
{
class Geometry : public Managed<OSPGeometry>
{
public:
    using Managed::Managed;
};

class GeometricModel : public Managed<OSPGeometricModel>
{
public:
    using Managed::Managed;

    void setGeometry(const Geometry &geometry);
    void setMaterial(std::uint32_t rendererIndex);
    void setPrimitiveMaterials(SharedArray<std::uint32_t> rendererIndices);
    void setPrimitiveColors(SharedArray<Color4> colors);
    void setColor(const Color4 &color);
    void removeColors();
    void invertNormals(bool inverted);
    void setId(std::uint32_t id);
};

class Mesh : public Geometry
{
public:
    static inline const std::string name = "mesh";

    using Geometry::Geometry;

    void setVertexPositions(SharedArray<Vector3> positions);
    void setVertexNormals(SharedArray<Vector3> normals);
    void setVertexColors(SharedArray<Color4> colors);
    void setTriangleIndices(SharedArray<Index3> indices);
    void setQuadIndices(SharedArray<Index4> indices);
    void setQuadSoup(bool quadSoup);
};

class Spheres : public Geometry
{
public:
    static inline const std::string name = "sphere";

    using Geometry::Geometry;

    void setPositions(SharedArray<Vector3> positions);
    void setRadii(SharedArray<float> radii);
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
    Bspline = OSP_BSPLINE,
};

class Curve : public Geometry
{
public:
    static inline const std::string name = "curve";

    using Geometry::Geometry;

    void setVertexPositionsAndRadii(SharedArray<PositionRadius> positionsRadii);
    void setVertexColors(SharedArray<Color4> colors);
    void setIndices(SharedArray<std::uint32_t> indices);
    void setType(CurveType type);
    void setBasis(CurveBasis basis);
};

class Boxes : public Geometry
{
public:
    static inline const std::string name = "box";

    using Geometry::Geometry;

    void setBoxes(SharedArray<Box3> boxes);
};

class Planes : public Geometry
{
public:
    static inline const std::string name = "plane";

    using Geometry::Geometry;

    void setCoefficients(SharedArray<Vector4> coefficients);
    void setBounds(SharedArray<Box3> bounds);
};

class Isosurfaces : public Geometry
{
public:
    static inline const std::string name = "isosurface";

    using Geometry::Geometry;

    void setVolume(const Volume &volume);
    void setIsovalues(SharedArray<float> values);
    void setIsovalue(float value);
};
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Geometry, OSP_GEOMETRY)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::GeometricModel, OSP_GEOMETRIC_MODEL)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Mesh, OSP_GEOMETRY)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Spheres, OSP_GEOMETRY)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Curve, OSP_GEOMETRY)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Boxes, OSP_GEOMETRY)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Planes, OSP_GEOMETRY)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Isosurfaces, OSP_GEOMETRY)
}
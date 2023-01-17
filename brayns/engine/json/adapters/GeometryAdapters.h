/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/geometry/types/BoundedPlane.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/geometry/types/Plane.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/geometry/types/TriangleMesh.h>
#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(Box)
BRAYNS_JSON_ADAPTER_ENTRY(min, "Minimum bound corner (bottom back left)")
BRAYNS_JSON_ADAPTER_ENTRY(max, "Maximum bound corner (top front right)")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(BoundedPlane)
BRAYNS_JSON_ADAPTER_ENTRY(coefficients, "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)")
BRAYNS_JSON_ADAPTER_ENTRY(bounds, "Axis-aligned bounds to limit the plane geometry")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Plane)
BRAYNS_JSON_ADAPTER_ENTRY(coefficients, "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Capsule)
BRAYNS_JSON_ADAPTER_ENTRY(p0, "Starting point of the capsule")
BRAYNS_JSON_ADAPTER_ENTRY(r0, "Capsule radius at p0")
BRAYNS_JSON_ADAPTER_ENTRY(p1, "Ending point of the capsule")
BRAYNS_JSON_ADAPTER_ENTRY(r1, "Capsule radius at p1")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Sphere)
BRAYNS_JSON_ADAPTER_ENTRY(center, "Sphere center point")
BRAYNS_JSON_ADAPTER_ENTRY(radius, "Sphere radius")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(TriangleMesh)
BRAYNS_JSON_ADAPTER_ENTRY(vertices, "Mesh vertex positions")
BRAYNS_JSON_ADAPTER_ENTRY(normals, "Mesh vertex normals", Required(false))
BRAYNS_JSON_ADAPTER_ENTRY(colors, "Mesh vertex colors", Required(false))
BRAYNS_JSON_ADAPTER_ENTRY(uvs, "Mesh vertex texture coordinates", Required(false))
BRAYNS_JSON_ADAPTER_ENTRY(indices, "Mesh triangle vertex indices")
BRAYNS_JSON_ADAPTER_END()

template<typename T>
struct GeometryWithColor
{
    T geometry;
    Vector4f color;
};

#define ADD_GEOMETRY_ADAPTER(TYPE) \
    BRAYNS_JSON_ADAPTER_BEGIN(GeometryWithColor<TYPE>) \
    BRAYNS_JSON_ADAPTER_ENTRY(geometry, "Geometry data") \
    BRAYNS_JSON_ADAPTER_ENTRY(color, "Geometry color") \
    BRAYNS_JSON_ADAPTER_END()

ADD_GEOMETRY_ADAPTER(BoundedPlane)
ADD_GEOMETRY_ADAPTER(Box)
ADD_GEOMETRY_ADAPTER(Plane)
ADD_GEOMETRY_ADAPTER(Capsule)
ADD_GEOMETRY_ADAPTER(Sphere)

#undef ADD_GEOMETRY_ADAPTER
}

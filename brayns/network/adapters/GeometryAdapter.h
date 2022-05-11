/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/geometries/Box.h>
#include <brayns/engine/geometries/Plane.h>
#include <brayns/engine/geometries/Primitive.h>
#include <brayns/engine/geometries/Sphere.h>
#include <brayns/engine/geometries/TriangleMesh.h>
#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(Box)
BRAYNS_JSON_ADAPTER_ENTRY(min, "Minimum bound corner (bottom back left)")
BRAYNS_JSON_ADAPTER_ENTRY(max, "Maximum bound corner (top front right)")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Plane)
BRAYNS_JSON_ADAPTER_ENTRY(coefficients, "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Primitive)
BRAYNS_JSON_ADAPTER_ENTRY(p0, "Starting point of the primitive")
BRAYNS_JSON_ADAPTER_ENTRY(r0, "Primitive radius at p0")
BRAYNS_JSON_ADAPTER_ENTRY(p1, "Ending point of the primitive")
BRAYNS_JSON_ADAPTER_ENTRY(r1, "Primitive radius at p1")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Sphere)
BRAYNS_JSON_ADAPTER_ENTRY(center, "Sphere center point")
BRAYNS_JSON_ADAPTER_ENTRY(radius, "Sphere radius")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(TriangleMesh)
BRAYNS_JSON_ADAPTER_ENTRY(vertices, "Mesh vertex positions")
BRAYNS_JSON_ADAPTER_ENTRY(normals, "Mesh vertex normals")
BRAYNS_JSON_ADAPTER_ENTRY(colors, "Mesh vertex colors")
BRAYNS_JSON_ADAPTER_ENTRY(uvs, "Mesh vertex texture coordinates")
BRAYNS_JSON_ADAPTER_ENTRY(indices, "Mesh triangle vertex indices")
BRAYNS_JSON_ADAPTER_END()

template<typename T>
struct AddGeometryProxy
{
    T geometry;
    Vector4f color;
};

#define ADD_GEOMETRY_ADAPTER(Type) \
    BRAYNS_JSON_ADAPTER_BEGIN(AddGeometryProxy<Type>) \
    BRAYNS_JSON_ADAPTER_ENTRY(geometry, "Geometry data") \
    BRAYNS_JSON_ADAPTER_ENTRY(color, "Geometry color") \
    BRAYNS_JSON_ADAPTER_END()

ADD_GEOMETRY_ADAPTER(Box)
ADD_GEOMETRY_ADAPTER(Plane)
ADD_GEOMETRY_ADAPTER(Primitive)
ADD_GEOMETRY_ADAPTER(Sphere)
}

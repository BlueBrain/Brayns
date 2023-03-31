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

#include <brayns/json/Json.h>

#include "BoundsAdapter.h"

namespace brayns
{
template<>
struct JsonAdapter<Box> : ObjectAdapter<Box>
{
    static void reflect()
    {
        title("Box");
        set<Vector3f>("min", [](auto &object, const auto &value) { object.min = value; })
            .description("Bottom back left corner XYZ");
        set<Vector3f>("max", [](auto &object, const auto &value) { object.max = value; })
            .description("Top front right corner XYZ");
    }
};

template<>
struct JsonAdapter<BoundedPlane> : ObjectAdapter<BoundedPlane>
{
    static void reflect()
    {
        title("BoundedPlane");
        set<Vector4f>("coefficients", [](auto &object, const auto &value) { object.coefficients = value; })
            .description("Equation coefficients ABCD from Ax + By + Cz + D = 0");
        set<Bounds>("bounds", [](auto &object, const auto &value) { object.bounds = value; })
            .description("Axis-aligned bounds to limit the plane geometry");
    }
};

template<>
struct JsonAdapter<Plane> : ObjectAdapter<Plane>
{
    static void reflect()
    {
        title("Plane");
        set<Vector4f>("coefficients", [](auto &object, const auto &value) { object.coefficients = value; })
            .description("Equation coefficients ABCD from Ax + By + Cz + D = 0");
    }
};

template<>
struct JsonAdapter<Capsule> : ObjectAdapter<Capsule>
{
    static void reflect()
    {
        title("Capsule");
        set<Vector3f>("p0", [](auto &object, const auto &value) { object.p0 = value; })
            .description("Start point of the capsule XYZ");
        set<float>("r0", [](auto &object, auto value) { object.r0 = value; }).description("Capsule radius at p0");
        set<Vector3f>("p1", [](auto &object, const auto &value) { object.p1 = value; })
            .description("End point of the capsule XYZ");
        set<float>("r1", [](auto &object, auto value) { object.r1 = value; }).description("Capsule radius at p1");
    }
};

template<>
struct JsonAdapter<Sphere> : ObjectAdapter<Sphere>
{
    static void reflect()
    {
        title("Sphere");
        set<Vector3f>("center", [](auto &object, const auto &value) { object.center = value; })
            .description("Sphere center XYZ");
        set<float>("radius", [](auto &object, auto value) { object.radius = value; }).description("Sphere radius");
    }
};

template<>
struct JsonAdapter<TriangleMesh> : ObjectAdapter<TriangleMesh>
{
    static void reflect()
    {
        title("TriangleMesh");
        set<std::vector<Vector3f>>("vertices", [](auto &object, auto value) { object.vertices = std::move(value); })
            .description("Mesh vertex positions");
        set<std::vector<Vector3f>>("normals", [](auto &object, auto value) { object.normals = std::move(value); })
            .description("Mesh vertex normals")
            .required(false);
        set<std::vector<Vector4f>>("colors", [](auto &object, auto value) { object.colors = std::move(value); })
            .description("Mesh vertex colors")
            .required(false);
        set<std::vector<Vector2f>>("uvs", [](auto &object, auto value) { object.uvs = std::move(value); })
            .description("Mesh vertex texture coordinates")
            .required(false);
        set<std::vector<Vector3ui>>("indices", [](auto &object, auto value) { object.indices = std::move(value); })
            .description("Mesh vertex triangle indices");
    }
};

template<typename T>
struct GeometryWithColor
{
    T geometry;
    Vector4f color;
};

template<typename T>
struct JsonAdapter<GeometryWithColor<T>> : ObjectAdapter<GeometryWithColor<T>>
{
    static void reflect()
    {
        title("GeometryWithColor");
        set<T>("geometry", [](auto &object, const auto &value) { object.geometry = value; })
            .description("Geometry data");
        set<Vector4f>("color", [](auto &object, const auto &value) { object.color = value; })
            .description("Geometry color");
    }
};

template<typename T>
struct ClippingGeometry
{
    std::vector<T> primitives;
    bool invertNormals;
};

template<typename T>
struct JsonAdapter<ClippingGeometry<T>> : ObjectAdapter<ClippingGeometry<T>>
{
    static void reflect()
    {
        title("ClippingGeometry");
        set<std::vector<T>>("primitives", [](auto &object, auto value) { object.primitives = std::move(value); })
            .description("Clipping primitive list");
        set<bool>("invert_normals", [](auto &object, auto value) { object.invertNormals = value; })
            .description("Switches clipping side")
            .defaultValue(false);
    }
};
} // namespace brayns

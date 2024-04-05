/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/engine/geometry/types/BoundedPlane.h>
#include <brayns/core/engine/geometry/types/Box.h>
#include <brayns/core/engine/geometry/types/Capsule.h>
#include <brayns/core/engine/geometry/types/Plane.h>
#include <brayns/core/engine/geometry/types/Sphere.h>
#include <brayns/core/engine/geometry/types/TriangleMesh.h>

#include <brayns/core/json/Json.h>

#include "BoundsAdapter.h"

namespace brayns
{
template<>
struct JsonAdapter<Box> : ObjectAdapter<Box>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Box");
        builder
            .getset(
                "min",
                [](auto &object) -> auto & { return object.min; },
                [](auto &object, const auto &value) { object.min = value; })
            .description("Bottom back left corner XYZ");
        builder
            .getset(
                "max",
                [](auto &object) -> auto & { return object.max; },
                [](auto &object, const auto &value) { object.max = value; })
            .description("Top front right corner XYZ");
        return builder.build();
    }
};

template<>
struct JsonAdapter<BoundedPlane> : ObjectAdapter<BoundedPlane>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("BoundedPlane");
        builder
            .getset(
                "coefficients",
                [](auto &object) -> auto & { return object.coefficients; },
                [](auto &object, const auto &value) { object.coefficients = value; })
            .description("Equation coefficients ABCD from Ax + By + Cz + D = 0");
        builder
            .getset(
                "bounds",
                [](auto &object) -> auto & { return object.bounds; },
                [](auto &object, const auto &value) { object.bounds = value; })
            .description("Axis-aligned bounds to limit the plane geometry");
        return builder.build();
    }
};

template<>
struct JsonAdapter<Plane> : ObjectAdapter<Plane>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Plane");
        builder
            .getset(
                "coefficients",
                [](auto &object) -> auto & { return object.coefficients; },
                [](auto &object, const auto &value) { object.coefficients = value; })
            .description("Equation coefficients ABCD from Ax + By + Cz + D = 0");
        return builder.build();
    }
};

template<>
struct JsonAdapter<Capsule> : ObjectAdapter<Capsule>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Capsule");
        builder
            .getset(
                "p0",
                [](auto &object) -> auto & { return object.p0; },
                [](auto &object, const auto &value) { object.p0 = value; })
            .description("Start point of the capsule XYZ");
        builder
            .getset(
                "r0",
                [](auto &object) { return object.r0; },
                [](auto &object, auto value) { object.r0 = value; })
            .description("Capsule radius at p0");
        builder
            .getset(
                "p1",
                [](auto &object) -> auto & { return object.p1; },
                [](auto &object, const auto &value) { object.p1 = value; })
            .description("End point of the capsule XYZ");
        builder
            .getset(
                "r1",
                [](auto &object) { return object.r1; },
                [](auto &object, auto value) { object.r1 = value; })
            .description("Capsule radius at p1");
        return builder.build();
    }
};

template<>
struct JsonAdapter<Sphere> : ObjectAdapter<Sphere>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Sphere");
        builder
            .getset(
                "center",
                [](auto &object) -> auto & { return object.center; },
                [](auto &object, const auto &value) { object.center = value; })
            .description("Sphere center XYZ");
        builder
            .getset(
                "radius",
                [](auto &object) { return object.radius; },
                [](auto &object, auto value) { object.radius = value; })
            .description("Sphere radius");
        return builder.build();
    }
};

template<>
struct JsonAdapter<TriangleMesh> : ObjectAdapter<TriangleMesh>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("TriangleMesh");
        builder
            .getset(
                "vertices",
                [](auto &object) -> auto & { return object.vertices; },
                [](auto &object, auto value) { object.vertices = std::move(value); })
            .description("Mesh vertex positions");
        builder
            .getset(
                "normals",
                [](auto &object) -> auto & { return object.normals; },
                [](auto &object, auto value) { object.normals = std::move(value); })
            .description("Mesh vertex normals")
            .required(false);
        builder
            .getset(
                "colors",
                [](auto &object) -> auto & { return object.colors; },
                [](auto &object, auto value) { object.colors = std::move(value); })
            .description("Mesh vertex colors")
            .required(false);
        builder
            .getset(
                "uvs",
                [](auto &object) -> auto & { return object.uvs; },
                [](auto &object, auto value) { object.uvs = std::move(value); })
            .description("Mesh vertex texture coordinates")
            .required(false);
        builder
            .getset(
                "indices",
                [](auto &object) -> auto & { return object.indices; },
                [](auto &object, auto value) { object.indices = std::move(value); })
            .description("Mesh vertex triangle indices");
        return builder.build();
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
    using Builder = JsonObjectBuilder<GeometryWithColor<T>>;

    static JsonObjectInfo reflect()
    {
        auto builder = Builder("GeometryWithColor");
        builder
            .getset(
                "geometry",
                [](auto &object) -> auto & { return object.geometry; },
                [](auto &object, const auto &value) { object.geometry = value; })
            .description("Geometry data");
        builder
            .getset(
                "color",
                [](auto &object) -> auto & { return object.color; },
                [](auto &object, const auto &value) { object.color = value; })
            .description("Geometry color");
        return builder.build();
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
    using Builder = JsonObjectBuilder<ClippingGeometry<T>>;

    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ClippingGeometry");
        builder
            .getset(
                "primitives",
                [](auto &object) -> auto & { return object.primitives; },
                [](auto &object, auto value) { object.primitives = std::move(value); })
            .description("Clipping primitive list");
        builder
            .getset(
                "invert_normals",
                [](auto &object) { return object.invertNormals; },
                [](auto &object, auto value) { object.invertNormals = value; })
            .description("Switches clipping side")
            .defaultValue(false);
        return builder.build();
    }
};
} // namespace brayns

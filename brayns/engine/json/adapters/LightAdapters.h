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

#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/light/types/QuadLight.h>
#include <brayns/engine/light/types/SphereLight.h>

#include <brayns/json/Json.h>

namespace brayns
{
class LightAdapter
{
public:
    template<typename T>
    static void reflect(JsonObjectBuilder<T> &builder)
    {
        builder
            .getset(
                "color",
                [](auto &object) -> auto & { return object.color; },
                [](auto &object, const auto &value) { object.color = value; })
            .description("Light color RGB normalized")
            .defaultValue(Vector3f(1));
        builder
            .getset(
                "intensity",
                [](auto &object) { return object.intensity; },
                [](auto &object, auto value) { object.intensity = value; })
            .description("Light intensity")
            .minimum(0)
            .defaultValue(1);
    }
};

template<>
struct JsonAdapter<AmbientLight> : ObjectAdapter<AmbientLight>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("AmbientLight");
        LightAdapter::reflect(builder);
        return builder.build();
    }
};

template<>
struct JsonAdapter<DirectionalLight> : ObjectAdapter<DirectionalLight>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("DirectionalLight");
        LightAdapter::reflect(builder);
        builder
            .getset(
                "direction",
                [](auto &object) -> auto & { return object.direction; },
                [](auto &object, const auto &value) { object.direction = value; })
            .description("Light direction XYZ")
            .defaultValue(Vector3f(-1, -1, 0));
        return builder.build();
    }
};

template<>
struct JsonAdapter<QuadLight> : ObjectAdapter<QuadLight>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("QuadLight");
        LightAdapter::reflect(builder);
        builder
            .getset(
                "position",
                [](auto &object) -> auto & { return object.position; },
                [](auto &object, const auto &value) { object.position = value; })
            .description("Light base corner position XYZ")
            .defaultValue(Vector3f(0));
        builder
            .getset(
                "edge1",
                [](auto &object) -> auto & { return object.edge1; },
                [](auto &object, const auto &value) { object.edge1 = value; })
            .description("Edge 1 XYZ")
            .defaultValue(Vector3f(1, 0, 0));
        builder
            .getset(
                "edge2",
                [](auto &object) -> auto & { return object.edge2; },
                [](auto &object, const auto &value) { object.edge2 = value; })
            .description("Edge 2 XYZ")
            .defaultValue(Vector3f(0, 0, 1));
        return builder.build();
    }
};

template<>
struct JsonAdapter<SphereLight> : ObjectAdapter<SphereLight>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SphereLight");
        LightAdapter::reflect(builder);
        builder
            .getset(
                "position",
                [](auto &object) -> auto & { return object.position; },
                [](auto &object, const auto &value) { object.position = value; })
            .description("Light position XYZ")
            .defaultValue(Vector3f(0));
        builder
            .getset(
                "radius",
                [](auto &object) -> auto & { return object.radius; },
                [](auto &object, const auto &value) { object.radius = value; })
            .description("Sphere radius")
            .defaultValue(0.0f);
        return builder.build();
    }
};
} // namespace brayns

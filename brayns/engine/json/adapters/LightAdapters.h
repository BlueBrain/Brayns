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

#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/light/types/QuadLight.h>

#include <brayns/json/Json.h>

namespace brayns
{
template<typename T>
struct LightAdapter : ObjectAdapter<T>
{
protected:
    static void reflectDefault()
    {
        set<Vector3f>("color", [](auto &object, const auto &value) { object.color = value; })
            .description("Light color RGB normalized")
            .required(false);
        set<float>("intensity", [](auto &object, auto value) { object.intensity = value; })
            .description("Light intensity")
            .minimum(0)
            .defaultValue(1);
    }
};

template<>
struct JsonAdapter<AmbientLight> : LightAdapter<AmbientLight>
{
    static void reflect()
    {
        title("AmbientLight");
        reflectDefault();
    }
};

template<>
struct JsonAdapter<DirectionalLight> : LightAdapter<DirectionalLight>
{
    static void reflect()
    {
        title("DirectionalLight");
        reflectDefault();
        set<Vector3f>("direction", [](auto &object, const auto &value) { object.direction = value; })
            .description("Light direction XYZ")
            .defaultValue(Vector3f(-1, -1, 0));
    }
};

template<>
struct JsonAdapter<QuadLight> : LightAdapter<QuadLight>
{
    static void reflect()
    {
        title("QuadLight");
        reflectDefault();
        set<Vector3f>("position", [](auto &object, const auto &value) { object.position = value; })
            .description("Light base corner position XYZ")
            .defaultValue(Vector3f(0));
        set<Vector3f>("edge1", [](auto &object, const auto &value) { object.edge1 = value; })
            .description("Edge 1 XYZ")
            .defaultValue(Vector3f(1, 0, 0));
        set<Vector3f>("edge2", [](auto &object, const auto &value) { object.edge2 = value; })
            .description("Edge 2 XYZ")
            .defaultValue(Vector3f(0, 0, 1));
    }
};
} // namespace brayns

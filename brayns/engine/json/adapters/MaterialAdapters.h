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

#include <brayns/engine/material/types/CarPaint.h>
#include <brayns/engine/material/types/Emissive.h>
#include <brayns/engine/material/types/Glass.h>
#include <brayns/engine/material/types/Matte.h>
#include <brayns/engine/material/types/Metal.h>
#include <brayns/engine/material/types/Phong.h>
#include <brayns/engine/material/types/Plastic.h>

#include <brayns/json/Json.h>

namespace brayns
{
template<>
struct JsonAdapter<CarPaint> : ObjectAdapter<CarPaint>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("CarPaint");
        builder
            .getset(
                "flake_density",
                [](auto &object) { return object.flakeDensity; },
                [](auto &object, auto value) { object.flakeDensity = value; })
            .description("Metal flake density")
            .minimum(0)
            .maximum(1)
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Emissive> : ObjectAdapter<Emissive>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Emissive");
        builder
            .getset(
                "color",
                [](auto &object) -> auto & { return object.color; },
                [](auto &object, const auto &value) { object.color = value; })
            .description("Emission color")
            .required(false);
        builder
            .getset(
                "intensity",
                [](auto &object) { return object.intensity; },
                [](auto &object, auto value) { object.intensity = value; })
            .description("Emission intensity")
            .minimum(0)
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Glass> : ObjectAdapter<Glass>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Glass");
        builder
            .getset(
                "index_of_refraction",
                [](auto &object) { return object.indexOfRefraction; },
                [](auto &object, auto value) { object.indexOfRefraction = value; })
            .description("Index of refraction of the glass")
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Matte> : ObjectAdapter<Matte>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Matte");
        builder
            .getset(
                "opacity",
                [](auto &object) { return object.opacity; },
                [](auto &object, auto value) { object.opacity = value; })
            .description("Surface opacity")
            .minimum(0)
            .maximum(1)
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Metal> : ObjectAdapter<Metal>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Metal");
        builder
            .getset(
                "roughness",
                [](auto &object) { return object.roughness; },
                [](auto &object, auto value) { object.roughness = value; })
            .description("Surface roughness")
            .minimum(0.01)
            .maximum(1)
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Phong> : ObjectAdapter<Phong>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Phong");
        builder
            .getset(
                "opacity",
                [](auto &object) { return object.opacity; },
                [](auto &object, auto value) { object.opacity = value; })
            .description("Surface opacity")
            .minimum(0)
            .maximum(1)
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Plastic> : ObjectAdapter<Plastic>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Plastic");
        builder
            .getset(
                "opacity",
                [](auto &object) { return object.opacity; },
                [](auto &object, auto value) { object.opacity = value; })
            .description("Surface opacity")
            .minimum(0)
            .maximum(1)
            .required(false);
        return builder.build();
    }
};
} // namespace brayns

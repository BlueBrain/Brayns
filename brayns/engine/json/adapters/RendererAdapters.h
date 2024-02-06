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

#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/renderer/types/Production.h>

#include <brayns/json/Json.h>

namespace brayns
{
class RendererAdapter
{
public:
    template<typename T>
    static void reflect(JsonObjectBuilder<T> &builder)
    {
        builder
            .getset(
                "samples_per_pixel",
                [](auto &object) { return object.samplesPerPixel; },
                [](auto &object, auto value) { object.samplesPerPixel = value; })
            .description("Ray samples per pixel")
            .minimum(1)
            .required(false);
        builder
            .getset(
                "max_ray_bounces",
                [](auto &object) { return object.maxRayBounces; },
                [](auto &object, auto value) { object.maxRayBounces = value; })
            .description("Maximum ray bounces")
            .required(false);
        builder
            .getset(
                "background_color",
                [](auto &object) -> auto & { return object.backgroundColor; },
                [](auto &object, const auto &value) { object.backgroundColor = value; })
            .description("Background color")
            .required(false);
    }
};

template<>
struct JsonAdapter<Interactive> : ObjectAdapter<Interactive>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Interactive");
        RendererAdapter::reflect(builder);
        builder
            .getset(
                "enable_shadows",
                [](auto &object) { return object.shadowsEnabled; },
                [](auto &object, auto value) { object.shadowsEnabled = value; })
            .description("Enable casted shadows when rendering")
            .required(false);
        builder
            .getset(
                "ao_samples",
                [](auto &object) { return object.aoSamples; },
                [](auto &object, auto value) { object.aoSamples = value; })
            .description("Ambient occlusion samples")
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Production> : ObjectAdapter<Production>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Production");
        RendererAdapter::reflect(builder);
        return builder.build();
    }
};
} // namespace brayns

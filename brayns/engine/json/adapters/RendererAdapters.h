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

#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/renderer/types/Production.h>

#include <brayns/json/Json.h>

namespace brayns
{
template<typename T>
struct RendererAdapter : ObjectAdapter<T>
{
protected:
    using ObjectAdapter<T>::getset;

    static void reflectDefault()
    {
        getset(
            "samples_per_pixel",
            [](auto &object) { return object.samplesPerPixel; },
            [](auto &object, auto value) { object.samplesPerPixel = value; })
            .description("Ray samples per pixel")
            .minimum(1)
            .required(false);
        getset(
            "max_ray_bounces",
            [](auto &object) { return object.maxRayBounces; },
            [](auto &object, auto value) { object.maxRayBounces = value; })
            .description("Maximum ray bounces")
            .required(false);
        getset(
            "background_color",
            [](auto &object) -> auto & { return object.backgroundColor; },
            [](auto &object, const auto &value) { object.backgroundColor = value; })
            .description("Background color")
            .required(false);
    }
};

template<>
struct JsonAdapter<Interactive> : RendererAdapter<Interactive>
{
    static void reflect()
    {
        title("Interactive");
        reflectDefault();
        getset(
            "enable_shadows",
            [](auto &object) { return object.shadowsEnabled; },
            [](auto &object, auto value) { object.shadowsEnabled = value; })
            .description("Enable casted shadows when rendering")
            .required(false);
        getset(
            "ao_samples",
            [](auto &object) { return object.aoSamples; },
            [](auto &object, auto value) { object.aoSamples = value; })
            .description("Ambient occlusion samples")
            .required(false);
    }
};

template<>
struct JsonAdapter<Production> : RendererAdapter<Production>
{
    static void reflect()
    {
        title("Production");
        reflectDefault();
    }
};
} // namespace brayns

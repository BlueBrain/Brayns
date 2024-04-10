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

#include <brayns/core/engine/material/types/CarPaint.h>
#include <brayns/core/engine/material/types/Emissive.h>
#include <brayns/core/engine/material/types/Glass.h>
#include <brayns/core/engine/material/types/Matte.h>
#include <brayns/core/engine/material/types/Metal.h>
#include <brayns/core/engine/material/types/Phong.h>
#include <brayns/core/engine/material/types/Plastic.h>
#include <brayns/core/engine/material/types/Principled.h>

#include <brayns/core/json/Json.h>

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

template<>
struct JsonAdapter<Principled> : ObjectAdapter<Principled>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Principled");

        builder
            .getset(
                "edge_color",
                [](auto &object) { return object.edgeColor; },
                [](auto &object, auto value) { object.edgeColor = value; })
            .description("Edge tint for metallic surfaces")
            .required(false);
        builder
            .getset(
                "metallic",
                [](auto &object) { return object.metallic; },
                [](auto &object, auto value) { object.metallic = value; })
            .description("Alpha parameter between dielectric and metallic")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "diffuse",
                [](auto &object) { return object.diffuse; },
                [](auto &object, auto value) { object.diffuse = value; })
            .description("Diffuse reflection weight")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "specular",
                [](auto &object) { return object.specular; },
                [](auto &object, auto value) { object.specular = value; })
            .description("Specular reflection/transmission weight")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "ior",
                [](auto &object) { return object.ior; },
                [](auto &object, auto value) { object.ior = value; })
            .description("Dielectric index of refraction")
            .minimum(1)
            .required(false);
        builder
            .getset(
                "transmission",
                [](auto &object) { return object.transmission; },
                [](auto &object, auto value) { object.transmission = value; })
            .description("Specular transmission weight")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "transmission_color",
                [](auto &object) { return object.transmissionColor; },
                [](auto &object, auto value) { object.transmissionColor = value; })
            .description("Transmission attenuation color")
            .required(false);
        builder
            .getset(
                "transmission_depth",
                [](auto &object) { return object.transmissionDepth; },
                [](auto &object, auto value) { object.transmissionDepth = value; })
            .description("Distance from surface at which the color will equal transmission color")
            .required(false);
        builder
            .getset(
                "roughness",
                [](auto &object) { return object.roughness; },
                [](auto &object, auto value) { object.roughness = value; })
            .description("Diffuse and specular reflection roughness")
            .required(false);
        builder
            .getset(
                "anisotropy",
                [](auto &object) { return object.anisotropy; },
                [](auto &object, auto value) { object.anisotropy = value; })
            .description("Specular anisotropy reflection weight (Specular highlights depends on surface type/shape)")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "anisotropy_rotation",
                [](auto &object) { return object.anisotropyRotation; },
                [](auto &object, auto value) { object.anisotropyRotation = value; })
            .description("Rotation of the specular anisotropy reflection effect")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "thin",
                [](auto &object) { return object.thin; },
                [](auto &object, auto value) { object.thin = value; })
            .description("Specified wether the object is solid or thin (hollow)")
            .required(false);
        builder
            .getset(
                "thickness",
                [](auto &object) { return object.thickness; },
                [](auto &object, auto value) { object.thickness = value; })
            .description("Thickness of the object if thin = true")
            .required(false);
        builder
            .getset(
                "back_light",
                [](auto &object) { return object.backLight; },
                [](auto &object, auto value) { object.backLight = value; })
            .description("For thin objects, weight of reflection and transmission (1 = 50/50, 2 = only transmission)")
            .minimum(0)
            .maximum(2)
            .required(false);
        builder
            .getset(
                "coat",
                [](auto &object) { return object.coat; },
                [](auto &object, auto value) { object.coat = value; })
            .description("Clear coat weight (thin lacquered/glossy layer on top of the surface)")
            .minimum(0)
            .maximum(1)
            .required(false);
        builder
            .getset(
                "coat_ior",
                [](auto &object) { return object.coatIor; },
                [](auto &object, auto value) { object.coatIor = value; })
            .description("Clear coat index of refraction")
            .required(false);
        builder
            .getset(
                "coat_color",
                [](auto &object) { return object.coatColor; },
                [](auto &object, auto value) { object.coatColor = value; })
            .description("Clear coat color")
            .required(false);
        builder
            .getset(
                "coat_thickness",
                [](auto &object) { return object.coatThickness; },
                [](auto &object, auto value) { object.coatThickness = value; })
            .description("Clear coat thickness")
            .required(false);
        builder
            .getset(
                "coat_roughness",
                [](auto &object) { return object.coatRoughness; },
                [](auto &object, auto value) { object.coatRoughness = value; })
            .description("Clear coat diffuse/specular reflection roughness")
            .required(false);
        builder
            .getset(
                "sheen",
                [](auto &object) { return object.sheen; },
                [](auto &object, auto value) { object.sheen = value; })
            .description("Sheen effect weight (fabric-like effect such as satin or velvet)")
            .required(false);
        builder
            .getset(
                "sheen_color",
                [](auto &object) { return object.sheenColor; },
                [](auto &object, auto value) { object.sheenColor = value; })
            .description("Sheen color")
            .required(false);
        builder
            .getset(
                "sheen_tint",
                [](auto &object) { return object.sheenTint; },
                [](auto &object, auto value) { object.sheenTint = value; })
            .description("Strenght of sheen color (0 = white, 1 = sheen color)")
            .required(false);
        builder
            .getset(
                "sheen_roughness",
                [](auto &object) { return object.sheenRoughness; },
                [](auto &object, auto value) { object.sheenRoughness = value; })
            .description("Sheen diffuse/specular reflection roughness")
            .required(false);

        return builder.build();
    }
};
} // namespace brayns

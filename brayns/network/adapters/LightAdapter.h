/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/engine/Light.h>

#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(LightType, {"sphere", LightType::SPHERE},
                         {"directional", LightType::DIRECTIONAL},
                         {"quad", LightType::QUAD},
                         {"spotlight", LightType::SPOTLIGHT},
                         {"ambient", LightType::AMBIENT})

#define BRAYNS_LIGHT_PROPERTIES()                                       \
    BRAYNS_JSON_ADAPTER_NAMED_ENTRY("color", _color, "Light color RGB") \
    BRAYNS_JSON_ADAPTER_NAMED_ENTRY("intensity", _intensity,            \
                                    "Light intensity 0-1")              \
    BRAYNS_JSON_ADAPTER_NAMED_ENTRY("is_visible", _isVisible,           \
                                    "Light is visible")

BRAYNS_JSON_ADAPTER_BEGIN(DirectionalLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("direction", _direction,
                                "Light source direction");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("angular_diameter", _angularDiameter,
                                "Angular diameter in degrees");
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(SphereLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("position", _position, "Light position XYZ");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("radius", _radius, "Sphere radius");
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(QuadLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("position", _position, "Light position XYZ");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("edge1", _edge1, "First edge XYZ");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("edge2", _edge2, "Second edge XYZ");
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(SpotLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("position", _position, "Light position XYZ");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("direction", _direction, "Spot direction XYZ");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("opening_angle", _openingAngle,
                                "Opening angle in degrees");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("penumbra_angle", _penumbraAngle,
                                "Penumbra angle in degrees");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("radius", _radius, "Spot radius");
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(AmbientLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns

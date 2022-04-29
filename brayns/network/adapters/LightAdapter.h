/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/lights/AmbientLight.h>
#include <brayns/engine/lights/DirectionalLight.h>
#include <brayns/engine/lights/QuadLight.h>

#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
#define BRAYNS_LIGHT_PROPERTIES() \
    BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Light color (Normalized RGB)") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "intensity", \
        getIntensity, \
        setIntensity, \
        "Light intensity (Will be clamped on the range [0.0, +infinity)") \
    BRAYNS_JSON_ADAPTER_GETSET("visible", isVisible, setVisible, "Sets wether the light should be visible on the scene")

BRAYNS_JSON_ADAPTER_BEGIN(AmbientLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(DirectionalLight)
BRAYNS_JSON_ADAPTER_GETSET("direction", getDirection, setDirection, "Light direction vector")
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(QuadLight)
BRAYNS_JSON_ADAPTER_GETSET(
    "bottom_left_corner",
    getBottomLeftCorner,
    setBottomLeftCorner,
    "Sets the bottom left corner position of the light (in world space coordinates)")
BRAYNS_JSON_ADAPTER_GETSET(
    "vertical_displacement",
    getVerticalDisplacement,
    setVerticalDisplacement,
    "Sets the vertical displacement vector used to compute the top left corner")
BRAYNS_JSON_ADAPTER_GETSET(
    "horizontal_displacement",
    getHorizontalDisplacement,
    setHorizontalDisplacement,
    "Sets the horizontal displacement vector used to compute the bottom right corner")
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

} // namespace brayns

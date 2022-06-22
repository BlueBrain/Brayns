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
    BRAYNS_JSON_ADAPTER_GETSET("intensity", getIntensity, setIntensity, "Light intensity", Minimum(0.0)) \
    BRAYNS_JSON_ADAPTER_GETSET("visible", isVisible, setVisible, "Sets wether the light should be visible on the scene")

BRAYNS_JSON_ADAPTER_BEGIN(AmbientLight)
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(DirectionalLight)
BRAYNS_JSON_ADAPTER_GETSET("direction", getDirection, setDirection, "Light direction vector")
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(QuadLight)
BRAYNS_JSON_ADAPTER_GETSET("position", getPosition, setPosition, "Sets the corner position of the quad light")
BRAYNS_JSON_ADAPTER_GETSET("edge1", getEdge1, setEdge1, "Sets one of the quad light edges")
BRAYNS_JSON_ADAPTER_GETSET("edge2", getEdge2, setEdge2, "Sets one of the quad light edges")
BRAYNS_LIGHT_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

#undef BRAYNS_LIGHT_PROPERTIES

} // namespace brayns

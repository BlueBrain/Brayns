/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/materials/CarPaintMaterial.h>
#include <brayns/engine/materials/DefaultMaterial.h>
#include <brayns/engine/materials/EmissiveMaterial.h>
#include <brayns/engine/materials/GlassMaterial.h>
#include <brayns/engine/materials/MatteMaterial.h>
#include <brayns/engine/materials/MetalMaterial.h>
#include <brayns/engine/materials/PlasticMaterial.h>
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(CarPaintMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("flake_density", getFlakesDensity, setFlakesDesnity,
                           "Normalized percentage of flakes on the surface. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(DefaultMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("opacity", getOpacity, setOpacity,
                           "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(EmissiveMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("intensity", getIntensity, setIntensity,
                           "Intensity of the light emitted. Will be clampled to the range [0.0, +infinite]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(GlassMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("index_of_refraction", getIndexOfRefraction, setIndexOfRefraction,
                           "Index of refraction of the glass")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(MatteMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("opacity", getOpacity, setOpacity,
                           "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(MetalMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("roughness", getRoughness, setRoughness,
                           "Surface roughness. Will be clamped on the range [0-1]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(PlasticMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("opacity", getOpacity, setOpacity,
                           "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()
}

/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(CarPaint)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("flake_density", flakeDensity, "Metal flakes density", Minimum(0), Maximum(1))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Emissive)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_ENTRY(intensity, "Emitted light intensity", Minimum(0))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Glass)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("index_of_refraction", indexOfRefraction, "Index of refraction of the glass")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Matte)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_ENTRY(opacity, "Opacity of the surface", Minimum(0), Maximum(1))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Metal)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_ENTRY(roughness, "Surface roughness", Minimum(0.01), Maximum(1))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Phong)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_ENTRY(opacity, "Opacity of the surface", Minimum(0), Maximum(1))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Plastic)
BRAYNS_JSON_ADAPTER_ENTRY(color, "Base color of the material")
BRAYNS_JSON_ADAPTER_ENTRY(opacity, "Opacity of the surface", Minimum(0), Maximum(1))
BRAYNS_JSON_ADAPTER_END()
}

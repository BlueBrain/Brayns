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

#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/renderer/types/Production.h>

#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(Interactive)
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("samples_per_pixel", samplesPerPixel, "Ray samples per pixel", Minimum(1))
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("max_ray_bounces", maxRayBounces, "Maximum ray bounces")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("background_color", backgroundColor, "Background color")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("enable_shadows", shadowsEnabled, "Enable casted shadows when rendering")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("ao_samples", aoSamples, "Ambient occlusion samples")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Production)
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("samples_per_pixel", samplesPerPixel, "Ray samples per pixel", Minimum(1))
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("max_ray_bounces", maxRayBounces, "Maximum ray bounces")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("background_color", backgroundColor, "Background color")
BRAYNS_JSON_ADAPTER_END()
}

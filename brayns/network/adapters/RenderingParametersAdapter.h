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

#include <brayns/json/JsonAdapterMacro.h>

#include <brayns/parameters/RenderingParameters.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(RenderingParameters)
BRAYNS_JSON_ADAPTER_GETSET("accumulation", getAccumulation, setAccumulation,
                           "Multiple render passes")
BRAYNS_JSON_ADAPTER_GETSET("background_color", getBackgroundColor,
                           setBackgroundColor, "Background color RGB")
BRAYNS_JSON_ADAPTER_GETSET("current", getCurrentRenderer, setCurrentRenderer,
                           "Current renderer name")
BRAYNS_JSON_ADAPTER_GETSET("head_light", getHeadLight, setHeadLight,
                           "Light source follows camera origin")
BRAYNS_JSON_ADAPTER_GETSET("max_accum_frames", getMaxAccumFrames,
                           setMaxAccumFrames, "Max render passes")
BRAYNS_JSON_ADAPTER_GETSET("samples_per_pixel", getSamplesPerPixel,
                           setSamplesPerPixel, "Samples per pixel")
BRAYNS_JSON_ADAPTER_GETSET("subsampling", getSubsampling, setSubsampling,
                           "Subsampling")
BRAYNS_JSON_ADAPTER_GET("types", getRenderers, "Available renderers")
BRAYNS_JSON_ADAPTER_GETSET("variance_threshold", getVarianceThreshold,
                           setVarianceThreshold, "Stop accumulation threshold")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns

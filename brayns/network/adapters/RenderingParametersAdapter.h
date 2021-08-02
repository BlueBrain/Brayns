/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/json/MessageAdapter.h>

#include <brayns/parameters/RenderingParameters.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(RenderingParameters)
BRAYNS_ADAPTER_GETSET(bool, "accumulation", getAccumulation, setAccumulation,
                      "Multiple render passes")
BRAYNS_ADAPTER_GETSET(Vector3d, "background_color", getBackgroundColor,
                      setBackgroundColor, "Background color RGB")
BRAYNS_ADAPTER_GETSET(std::string, "current", getCurrentRenderer,
                      setCurrentRenderer, "Current renderer name")
BRAYNS_ADAPTER_GETSET(bool, "head_light", getHeadLight, setHeadLight,
                      "Light source follows camera origin")
BRAYNS_ADAPTER_GETSET(size_t, "max_accum_frames", getMaxAccumFrames,
                      setMaxAccumFrames, "Max render passes")
BRAYNS_ADAPTER_GETSET(uint32_t, "samples_per_pixel", getSamplesPerPixel,
                      setSamplesPerPixel, "Samples per pixel")
BRAYNS_ADAPTER_GETSET(uint32_t, "subsampling", getSubsampling, setSubsampling,
                      "Subsampling")
BRAYNS_ADAPTER_GET(std::vector<std::string>, "types", getRenderers,
                   "Available renderers")
BRAYNS_ADAPTER_GETSET(double, "variance_threshold", getVarianceThreshold,
                      setVarianceThreshold, "Stop accumulation threshold")
BRAYNS_ADAPTER_END()
} // namespace brayns
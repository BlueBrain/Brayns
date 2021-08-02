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

#include <brayns/parameters/VolumeParameters.h>

#include "BoxAdapter.h"

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(VolumeParameters)
BRAYNS_ADAPTER_GETSET(Vector3ui, "volume_dimensions", getDimensions,
                      setDimensions, "Dimensions XYZ")
BRAYNS_ADAPTER_GETSET(Vector3d, "volume_element_spacing", getElementSpacing,
                      setElementSpacing, "Element spacing XYZ")
BRAYNS_ADAPTER_GETSET(Vector3d, "volume_offset", getOffset, setOffset,
                      "Offset XYZ")
BRAYNS_ADAPTER_GETSET(bool, "gradient_shading", getGradientShading,
                      setGradientShading, "Use gradient shading")
BRAYNS_ADAPTER_GETSET(bool, "single_shade", getSingleShade, setSingleShade,
                      "Use a single shade for the whole volume")
BRAYNS_ADAPTER_GETSET(bool, "pre_integration", getPreIntegration,
                      setPreIntegration, "Use pre-integration")
BRAYNS_ADAPTER_GET(double, "adaptive_max_sampling_rate",
                   getAdaptiveMaxSamplingRate, "Max sampling rate")
BRAYNS_ADAPTER_GETSET(bool, "adaptive_sampling", getAdaptiveSampling,
                      setAdaptiveSampling, "Use adaptive sampling")
BRAYNS_ADAPTER_GETSET(double, "sampling_rate", getSamplingRate, setSamplingRate,
                      "Fixed sampling rate")
BRAYNS_ADAPTER_GETSET(Vector3d, "specular", getSpecular, setSpecular,
                      "Reflectivity amount XYZ")
BRAYNS_ADAPTER_GETSET(Boxd, "clip_box", getClipBox, setClipBox, "Clip box")
BRAYNS_ADAPTER_END()
} // namespace brayns
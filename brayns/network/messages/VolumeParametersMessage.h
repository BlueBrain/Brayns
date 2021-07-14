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

#include <brayns/network/message/Message.h>

#include "BoxMessage.h"

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(VolumeParametersMessage)
BRAYNS_MESSAGE_ENTRY(Vector3d, volume_dimensions, "Dimensions XYZ");
BRAYNS_MESSAGE_ENTRY(Vector3d, volume_element_spacing, "Element spacing XYZ");
BRAYNS_MESSAGE_ENTRY(Vector3d, volume_offset, "Offset XYZ");
BRAYNS_MESSAGE_ENTRY(bool, gradient_shading, "Use gradient shading");
BRAYNS_MESSAGE_ENTRY(bool, single_shade, "Use a single shade");
BRAYNS_MESSAGE_ENTRY(bool, pre_integration, "Use pre-integration");
BRAYNS_MESSAGE_ENTRY(double, adaptive_max_sampling_rate,
                     "Max sampling rate if adaptative");
BRAYNS_MESSAGE_ENTRY(bool, adaptive_sampling, "Use adaptive sampling");
BRAYNS_MESSAGE_ENTRY(double, sampling_rate, "Fixed sampling rate");
BRAYNS_MESSAGE_ENTRY(Vector3d, specular, "Reflectivity amount XYZ");
BRAYNS_MESSAGE_ENTRY(BoxMessage, clip_box, "Clip box");
BRAYNS_MESSAGE_END()
} // namespace brayns
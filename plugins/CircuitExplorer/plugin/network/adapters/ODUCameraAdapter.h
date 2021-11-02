/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include <brayns/json/MessageAdapter.h>

#include <plugin/api/CameraUtils.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(ODUCameraInformation)
BRAYNS_ADAPTER_NAMED_ENTRY("origin", origin,
                           "3D Global position at which to position the camera")
BRAYNS_ADAPTER_NAMED_ENTRY(
    "direction", direction,
    "3D vector indicating the direction the camera is facing")
BRAYNS_ADAPTER_NAMED_ENTRY("up", up,
                           "3D vector indicating the camera up direction to "
                           "build the orthornormal basis")
BRAYNS_ADAPTER_NAMED_ENTRY("aperture_radius", apertureRadius,
                           "Camera lens aperture radius (in mm)")
BRAYNS_ADAPTER_NAMED_ENTRY("focus_distance", focusDistance,
                           "Camera focus distance (in world units)")
BRAYNS_ADAPTER_END()
} // namespace brayns

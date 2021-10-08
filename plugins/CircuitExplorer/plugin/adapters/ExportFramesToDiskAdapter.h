/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <brayns/network/json/MessageAdapter.h>

#include <plugin/api/CircuitExplorerParams.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(ExportFramesToDisk)
BRAYNS_ADAPTER_NAMED_ENTRY("path", path, "Directory to store the frames")
BRAYNS_ADAPTER_NAMED_ENTRY("format", format, "The image format (PNG or JPEG)")
BRAYNS_ADAPTER_NAMED_ENTRY(
    "name_after_step", nameAfterStep,
    "Name the file on disk after the simulation step index")
BRAYNS_ADAPTER_NAMED_ENTRY("quality", quality,
                           "The quality at which the images will be stored")
BRAYNS_ADAPTER_NAMED_ENTRY("spp", spp, "Samples per pixels")
BRAYNS_ADAPTER_NAMED_ENTRY("start_frame", startFrame,
                           "The frame at which to start exporting frames")
BRAYNS_ADAPTER_NAMED_ENTRY("animation_information", animationInformation,
                           "A list of frame numbers to render")
BRAYNS_ADAPTER_NAMED_ENTRY(
    "camera_information", cameraInformation,
    "A list of camera definitions. Each camera definition contains origin, "
    "direction, up, apperture and radius. (1 entry per animation information "
    "entry)")
BRAYNS_ADAPTER_END()
} // namespace brayns
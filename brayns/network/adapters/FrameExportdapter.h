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

#include <brayns/engine/FrameExporter.h>
#include <brayns/network/adapters/CameraAdapter.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(FrameExporter::KeyFrame)
BRAYNS_ADAPTER_NAMED_ENTRY("frame_index", frameIndex,
                           "Integer index of the simulation frame")
BRAYNS_ADAPTER_NAMED_ENTRY("camera", camera, "Camera definition")
BRAYNS_ADAPTER_NAMED_ENTRY("camera_params", cameraParameters,
                           "Camera parameters")
BRAYNS_ADAPTER_END()

BRAYNS_ADAPTER_BEGIN(FrameExporter::ExportInfo)
BRAYNS_ADAPTER_NAMED_ENTRY("path", storePath,
                           "Path where to store the rendered frames")
BRAYNS_ADAPTER_NAMED_ENTRY("format", imageFormat,
                           "Image store format ('png', 'jpg', ...)")
BRAYNS_ADAPTER_NAMED_ENTRY("quality", imageQuality,
                           "Image quality (compression rate = 100 - quality, "
                           "100 = highest quality, 0 = lowest quality")
BRAYNS_ADAPTER_NAMED_ENTRY("spp", numSamples, "Number of samples per pixel")
BRAYNS_ADAPTER_NAMED_ENTRY("name_image_after_simulation_index",
                           nameImageAfterSimulationFrameIndex,
                           "Wether to name the rendered images after the "
                           "simulation frame index or not")
BRAYNS_ADAPTER_NAMED_ENTRY("key_frames", keyFrames,
                           "List of key frames to render")
BRAYNS_ADAPTER_END()
} // namespace brayns

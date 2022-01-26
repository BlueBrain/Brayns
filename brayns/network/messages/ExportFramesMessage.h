/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <brayns/common/adapters/PropertyMapAdapter.h>

#include <brayns/json/JsonObjectMacro.h>

#include <brayns/network/adapters/AnimationParametersAdapter.h>
#include <brayns/network/adapters/CameraAdapter.h>
#include <brayns/network/adapters/RenderingParametersAdapter.h>
#include <brayns/network/adapters/VolumeParametersAdapter.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(ExportFramesKeyFrame)
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, frame_index, "Integer index of the simulation frame")
BRAYNS_JSON_OBJECT_ENTRY(Camera, camera, "Camera definition")
BRAYNS_JSON_OBJECT_ENTRY(PropertyMap, camera_params, "Camera-specific parameters")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(ExportFramesParams)
BRAYNS_JSON_OBJECT_ENTRY(std::string, path, "Path where the frames will be stored")
BRAYNS_JSON_OBJECT_ENTRY(std::string, format, "Image format ('png' or 'jpg')")
BRAYNS_JSON_OBJECT_ENTRY(
    size_t,
    quality,
    "Image quality "
    "(100 = highest quality, 0 = lowest quality)")
BRAYNS_JSON_OBJECT_ENTRY(
    Vector2ui,
    image_size,
    "Image dimenssions "
    "[width, height]")
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, spp, "Samples per pixel")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<ExportFramesKeyFrame>, key_frames, "List of keyframes to export")
BRAYNS_JSON_OBJECT_ENTRY(std::string, renderer_name, "Name of the renderer to use", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(PropertyMap, renderer_parameters, "Renderer-specific parameters", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::unique_ptr<VolumeParameters>,
    volume_parameters,
    "Volume rendering parameters",
    Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    bool,
    name_after_simulation_index,
    "Name the frame image file after the simulation "
    "frame index",
    Default(false))
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(ExportFramesResult)
BRAYNS_JSON_OBJECT_ENTRY(int32_t, error, "Error code (0 = no error)")
BRAYNS_JSON_OBJECT_ENTRY(std::string, message, "Message explaining the error")
BRAYNS_JSON_OBJECT_END()

} // namespace brayns

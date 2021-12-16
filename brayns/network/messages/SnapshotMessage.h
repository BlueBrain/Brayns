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

#include <brayns/json/JsonObjectMacro.h>

#include <brayns/network/adapters/AnimationParametersAdapter.h>
#include <brayns/network/adapters/CameraAdapter.h>
#include <brayns/network/adapters/RenderingParametersAdapter.h>
#include <brayns/network/adapters/VolumeParametersAdapter.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(SnapshotParams)
BRAYNS_JSON_OBJECT_ENTRY(std::unique_ptr<AnimationParameters>,
                         animation_parameters, "Animation parameters",
                         Required(false))
BRAYNS_JSON_OBJECT_ENTRY(std::unique_ptr<VolumeParameters>, volume_parameters,
                         "Volume parameters", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(std::unique_ptr<Camera>, camera, "Camera parameters",
                         Required(false));
BRAYNS_JSON_OBJECT_ENTRY(std::string, format,
                         "Image format (extension without the dot)");
BRAYNS_JSON_OBJECT_ENTRY(std::string, name, "Name of the snapshot",
                         Required(false));
BRAYNS_JSON_OBJECT_ENTRY(size_t, quality, "Image quality from 0 to 100",
                         Required(false));
BRAYNS_JSON_OBJECT_ENTRY(std::unique_ptr<RenderingParameters>, renderer,
                         "Renderer parameters", Required(false));
BRAYNS_JSON_OBJECT_ENTRY(uint16_t, samples_per_pixel, "Samples per pixel",
                         Required(false));
BRAYNS_JSON_OBJECT_ENTRY(Vector2ui, size, "Image dimensions");
BRAYNS_JSON_OBJECT_ENTRY(std::string, file_path,
                         "Path if saved on disk. If empty, image will be sent"
                         "to the client as a base64 encoded image",
                         Required(false));
BRAYNS_JSON_OBJECT_END()

} // namespace brayns

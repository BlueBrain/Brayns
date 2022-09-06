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

#include <brayns/json/JsonObjectMacro.h>

#include <brayns/engine/json/adapters/EngineObjectDataAdapter.h>
#include <brayns/engine/json/adapters/ViewAdapter.h>
#include <brayns/network/messages/ImageSettingsMessage.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(ExportFramesKeyFrame)
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, frame_index, "Integer index of the simulation frame")
BRAYNS_JSON_OBJECT_ENTRY(std::optional<View>, camera_view, "Camera view settings", Required(false))
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(ExportFramesParams)
BRAYNS_JSON_OBJECT_ENTRY(std::string, path, "Path where the frames will be stored")
BRAYNS_JSON_OBJECT_ENTRY(ImageSettings, image_settings, "Image settings", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(EngineObjectData, camera, "Camera definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(EngineObjectData, renderer, "Renderer definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(std::vector<ExportFramesKeyFrame>, key_frames, "List of keyframes to export")
BRAYNS_JSON_OBJECT_ENTRY(bool, sequential_naming, "Name the image file after the frame index", Default(true))
BRAYNS_JSON_OBJECT_END()
} // namespace brayns

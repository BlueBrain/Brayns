/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <brayns/engine/json/adapters/EngineObjectDataAdapter.h>
#include <brayns/engine/json/adapters/ViewAdapter.h>
#include <brayns/network/adapters/SimulationParametersAdapter.h>
#include <brayns/network/messages/ImageSettingsMessage.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(SnapshotParams)
BRAYNS_JSON_OBJECT_ENTRY(ImageSettings, image_settings, "Image settings", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(EngineObjectData, camera, "Camera definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(View, camera_view, "Camera view settings", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(EngineObjectData, renderer, "Renderer definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, simulation_frame, "Simulation frame to render", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    file_path,
    "Snapshot will be saved at this path if specified, otherwise it will be returned as binary data with format from "
    "image_settings",
    Required(false))
BRAYNS_JSON_OBJECT_END()

#define BRAYNS_BUFFER_PROPERTIES() \
    BRAYNS_JSON_OBJECT_ENTRY(size_t, offset, "Buffer data offset in attached binary") \
    BRAYNS_JSON_OBJECT_ENTRY(size_t, size, "Buffer data size in attached binary")

BRAYNS_JSON_OBJECT_BEGIN(ColorBufferMessage)
BRAYNS_BUFFER_PROPERTIES()
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(SnapshotResult)
BRAYNS_JSON_OBJECT_ENTRY(ColorBufferMessage, color_buffer, "Snapshot color buffer encoded in params format")
BRAYNS_JSON_OBJECT_END()

#undef BRAYNS_BUFFER_PROPERTIES
} // namespace brayns

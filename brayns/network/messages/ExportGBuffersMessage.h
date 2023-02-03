/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerero@epfl.ch>
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
#include <brayns/engine/json/adapters/FramebufferChannelAdapter.h>
#include <brayns/engine/json/adapters/ViewAdapter.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(GBuffersParams)
BRAYNS_JSON_OBJECT_ENTRY(Vector2ui, resolution, "Image resolution", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(EngineObjectData, camera, "Camera definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(View, camera_view, "Camera view settings", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(EngineObjectData, renderer, "Renderer definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, simulation_frame, "Simulation frame to render", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    file_path,
    "Buffers will be saved at this path if specified, otherwise it will be returned as EXR encoded binary data",
    Required(false))
BRAYNS_JSON_OBJECT_ENTRY(std::vector<FramebufferChannel>, channels, "G buffer channels to export")
BRAYNS_JSON_OBJECT_END()
}

/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
#include <brayns/network/adapters/RendererAdapter.h>
#include <brayns/network/common/EngineObjectFactory.h>
#include <brayns/network/messages/ImageSettingsMessage.h>

#include <optional>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(SnapshotParams)
BRAYNS_JSON_OBJECT_ENTRY(ImageSettings, image_settings, "Image settings", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(GenericObject<Camera>, camera, "Camera definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(LookAt, camera_view, "Camera 'look at' view settings", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(GenericObject<Renderer>, renderer, "Renderer definition", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, simulation_frame, "Simulation frame to render", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    file_path,
    "Path if saved on disk. If empty, image will be sent to the client as a base64 encoded image",
    Required(false))
BRAYNS_JSON_OBJECT_END()

} // namespace brayns

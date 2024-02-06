/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/json/Json.h>

#include <brayns/engine/json/adapters/EngineObjectDataAdapter.h>
#include <brayns/engine/json/adapters/FramebufferChannelAdapter.h>
#include <brayns/engine/json/adapters/ViewAdapter.h>

namespace brayns
{
struct GBuffersParams
{
    Vector2ui resolution{0};
    EngineObjectData camera;
    View camera_view;
    EngineObjectData renderer;
    uint32_t simulation_frame = 0;
    std::string file_path;
    std::vector<FramebufferChannel> channels;
};

template<>
struct JsonAdapter<GBuffersParams> : ObjectAdapter<GBuffersParams>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("GBuffersParams");
        builder
            .getset(
                "resolution",
                [](auto &object) -> auto & { return object.resolution; },
                [](auto &object, const auto &value) { object.resolution = value; })
            .description("Image resolution")
            .required(false);
        builder
            .getset(
                "camera",
                [](auto &object) -> auto & { return object.camera; },
                [](auto &object, const auto &value) { object.camera = value; })
            .description("Camera definition")
            .required(false);
        builder
            .getset(
                "camera_view",
                [](auto &object) -> auto & { return object.camera_view; },
                [](auto &object, const auto &value) { object.camera_view = value; })
            .description("Camera view")
            .required(false);
        builder
            .getset(
                "renderer",
                [](auto &object) -> auto & { return object.renderer; },
                [](auto &object, const auto &value) { object.renderer = value; })
            .description("Renderer")
            .required(false);
        builder
            .getset(
                "simulation_frame",
                [](auto &object) { return object.simulation_frame; },
                [](auto &object, auto value) { object.simulation_frame = value; })
            .description("Simulation frame to render")
            .required(false);
        builder
            .getset(
                "file_path",
                [](auto &object) -> auto & { return object.file_path; },
                [](auto &object, auto value) { object.file_path = std::move(value); })
            .description("Path to save the buffer as EXR, encoded data is returned if unset")
            .required(false);
        builder
            .getset(
                "channels",
                [](auto &object) -> auto & { return object.channels; },
                [](auto &object, auto value) { object.channels = std::move(value); })
            .description("Framebuffer channels to export");
        return builder.build();
    }
};
} // namespace brayns

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

#include <brayns/json/Json.h>

#include <brayns/engine/json/adapters/EngineObjectDataAdapter.h>
#include <brayns/engine/json/adapters/ViewAdapter.h>
#include <brayns/network/messages/ImageSettingsMessage.h>

namespace brayns
{
struct SnapshotParams
{
    ImageSettings image_settings;
    EngineObjectData camera;
    View camera_view;
    EngineObjectData renderer;
    uint32_t simulation_frame;
    std::string file_path;
};

template<>
struct JsonAdapter<SnapshotParams> : ObjectAdapter<SnapshotParams>
{
    static void reflect()
    {
        title("SnapshotParams");
        getset(
            "image_settings",
            [](auto &object) -> auto & { return object.image_settings; },
            [](auto &object, auto value) { object.image_settings = std::move(value); })
            .description("Image settings")
            .required(false);
        getset(
            "camera",
            [](auto &object) -> auto & { return object.camera; },
            [](auto &object, const auto &value) { object.camera = value; })
            .description("Camera definition")
            .required(false);
        getset(
            "camera_view",
            [](auto &object) -> auto & { return object.camera_view; },
            [](auto &object, const auto &value) { object.camera_view = value; })
            .description("Camera view")
            .required(false);
        getset(
            "renderer",
            [](auto &object) -> auto & { return object.renderer; },
            [](auto &object, const auto &value) { object.renderer = value; })
            .description("Renderer definition")
            .required(false);
        getset(
            "simulation_frame",
            [](auto &object) { return object.simulation_frame; },
            [](auto &object, auto value) { object.simulation_frame = value; })
            .description("Simulation frame to render")
            .required(false);
        getset(
            "file_path",
            [](auto &object) -> auto & { return object.file_path; },
            [](auto &object, auto value) { object.file_path = std::move(value); })
            .description("Path to save image, raw encoded data will be returned if empty")
            .required(false);
    }
};

struct ColorBufferMessage
{
    size_t offset = 0;
    size_t size = 0;
};

template<>
struct JsonAdapter<ColorBufferMessage> : ObjectAdapter<ColorBufferMessage>
{
    static void reflect()
    {
        title("ColorBufferMessage");
        get("offset", [](auto &object) { return object.offset; }).description("Data offset in attached binary");
        get("size", [](auto &object) { return object.size; }).description("Data size in attached binary");
    }
};

struct SnapshotResult
{
    ColorBufferMessage color_buffer;
};

template<>
struct JsonAdapter<SnapshotResult> : ObjectAdapter<SnapshotResult>
{
    static void reflect()
    {
        title("SnapshotResult");
        get("color_buffer", [](auto &object) { return object.color_buffer; })
            .description("Encoded snapshot color buffer");
    }
};
} // namespace brayns

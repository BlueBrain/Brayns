/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/json/Json.h>

#include <brayns/core/engine/json/adapters/EngineObjectDataAdapter.h>
#include <brayns/core/engine/json/adapters/ViewAdapter.h>
#include <brayns/core/network/adapters/ImageMetadataAdapter.h>
#include <brayns/core/network/messages/ImageSettingsMessage.h>

namespace brayns
{
struct SnapshotParams
{
    ImageSettings image_settings;
    EngineObjectData camera;
    View camera_view;
    float camera_near_clip = 0.0f;
    EngineObjectData renderer;
    uint32_t simulation_frame;
    std::string file_path;
    std::optional<ImageMetadata> metadata;
};

template<>
struct JsonAdapter<SnapshotParams> : ObjectAdapter<SnapshotParams>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SnapshotParams");
        builder
            .getset(
                "image_settings",
                [](auto &object) -> auto & { return object.image_settings; },
                [](auto &object, auto value) { object.image_settings = std::move(value); })
            .description("Image settings")
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
                "camera_near_clip",
                [](auto &object) -> auto & { return object.camera_near_clip; },
                [](auto &object, const auto &value) { object.camera_near_clip = value; })
            .description("Camera near clipping distance")
            .required(false);
        builder
            .getset(
                "renderer",
                [](auto &object) -> auto & { return object.renderer; },
                [](auto &object, const auto &value) { object.renderer = value; })
            .description("Renderer definition")
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
            .description("Path to save image, raw encoded data will be returned if empty")
            .required(false);
        builder
            .getset(
                "metadata",
                [](auto &object) -> auto & { return object.metadata; },
                [](auto &object, auto value) { object.metadata = std::move(value); })
            .description("Metadata information to embed into the image")
            .required(false);
        return builder.build();
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
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ColorBufferMessage");
        builder.get("offset", [](auto &object) { return object.offset; }).description("Data offset in attached binary");
        builder.get("size", [](auto &object) { return object.size; }).description("Data size in attached binary");
        return builder.build();
    }
};

struct SnapshotResult
{
    ColorBufferMessage color_buffer;
};

template<>
struct JsonAdapter<SnapshotResult> : ObjectAdapter<SnapshotResult>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SnapshotResult");
        builder.get("color_buffer", [](auto &object) { return object.color_buffer; })
            .description("Encoded snapshot color buffer");
        return builder.build();
    }
};
} // namespace brayns

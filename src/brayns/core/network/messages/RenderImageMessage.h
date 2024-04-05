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

namespace brayns
{
struct RenderImageParams
{
    bool send = false;
    bool force = false;
    bool accumulate = false;
    std::string format;
    int jpeg_quality = 0;
    bool render = false;
};

template<>
struct JsonAdapter<RenderImageParams> : ObjectAdapter<RenderImageParams>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("RenderImageParams");
        builder
            .getset(
                "send",
                [](auto &object) { return object.send; },
                [](auto &object, auto value) { object.send = value; })
            .description("Send image once rendered")
            .defaultValue(true);
        builder
            .getset(
                "force",
                [](auto &object) { return object.force; },
                [](auto &object, auto value) { object.force = value; })
            .description("Send image even if nothing new was rendered")
            .defaultValue(false);
        builder
            .getset(
                "accumulate",
                [](auto &object) { return object.accumulate; },
                [](auto &object, auto value) { object.accumulate = value; })
            .description("Render all images until max accumulation")
            .defaultValue(false);
        builder
            .getset(
                "format",
                [](auto &object) -> auto & { return object.format; },
                [](auto &object, auto value) { object.format = std::move(value); })
            .description("Encoding of returned image data (jpg or png)")
            .defaultValue("jpg");
        builder
            .getset(
                "jpeg_quality",
                [](auto &object) { return object.jpeg_quality; },
                [](auto &object, auto value) { object.jpeg_quality = value; })
            .description("Quality if using JPEG encoding")
            .minimum(0)
            .maximum(100)
            .defaultValue(100);
        builder
            .getset(
                "render",
                [](auto &object) { return object.render; },
                [](auto &object, auto value) { object.render = value; })
            .description("Disable render if set to false (download only)")
            .defaultValue(true);
        return builder.build();
    }
};

struct RenderImageResult
{
    size_t accumulation = 0;
    size_t max_accumulation = 0;
};

template<>
struct JsonAdapter<RenderImageResult> : ObjectAdapter<RenderImageResult>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("RenderImageResult");
        builder.get("accumulation", [](auto &object) { return object.accumulation; })
            .description("Current frame accumulation");
        builder.get("max_accumulation", [](auto &object) { return object.max_accumulation; })
            .description("Maximum frame accumulation");
        return builder.build();
    }
};
} // namespace brayns

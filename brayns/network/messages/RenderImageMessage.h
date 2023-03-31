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

namespace brayns
{
struct RenderImageParams
{
    bool send = false;
    bool force = false;
    bool accumulate = false;
    std::string format;
    int jpeg_quality = 0;
};

template<>
struct JsonAdapter<RenderImageParams> : ObjectAdapter<RenderImageParams>
{
    static void reflect()
    {
        title("RenderImageParams");
        set<bool>("send", [](auto &object, auto value) { object.send = value; })
            .description("Send image once rendered")
            .defaultValue(true);
        set<bool>("force", [](auto &object, auto value) { object.force = value; })
            .description("Send image even if nothing new was rendered")
            .defaultValue(false);
        set<bool>("accumulate", [](auto &object, auto value) { object.accumulate = value; })
            .description("Render all images until max accumulation")
            .defaultValue(false);
        set<std::string>("format", [](auto &object, auto value) { object.format = std::move(value); })
            .description("Encoding of returned image data (jpg or png)")
            .defaultValue("jpg");
        set<int>("jpeg_quality", [](auto &object, auto value) { object.jpeg_quality = value; })
            .description("Quality if using JPEG encoding")
            .minimum(0)
            .maximum(100)
            .defaultValue(100);
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
    static void reflect()
    {
        title("RenderImageResult");
        get("accumulation", [](auto &object, auto value) { object.accumulation = value; })
            .description("Current frame accumulation");
        get("max_accumulation", [](auto &object, auto value) { object.max_accumulation = value; })
            .description("Maximum frame accumulation");
    }
};
} // namespace brayns

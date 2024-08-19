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

#include <variant>

#include <brayns/core/json/Json.h>

namespace brayns
{
struct JpegCodec
{
    int quality;
};

template<>
struct JsonObjectReflector<JpegCodec>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JpegCodec>();
        builder.constant("type", "Jpeg");
        builder.field("quality", [](auto &object) { return &object.quality; })
            .description(
                "JPEG quality, 1 = worst quality but best compression, 100 = best quality but worst compression")
            .defaultValue(100)
            .minimum(1)
            .maximum(100);
        return builder.build();
    }
};

struct PngCodec
{
};

template<>
struct JsonObjectReflector<PngCodec>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PngCodec>();
        builder.constant("type", "Png");
        return builder.build();
    }
};

struct ExrCodec
{
};

template<>
struct JsonObjectReflector<ExrCodec>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ExrCodec>();
        builder.constant("type", "Exr");
        return builder.build();
    }
};

using Codec = std::variant<std::monostate, JpegCodec, PngCodec, ExrCodec>;
}

/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/engine/framebuffer/Framebuffer.h>

#include <brayns/core/json/Json.h>

#include <optional>

namespace brayns
{
class ImageSettings
{
public:
    ImageSettings() = default;

    explicit ImageSettings(const Vector2ui &baseSize):
        _size(baseSize)
    {
    }

    const Vector2ui &getSize() const noexcept
    {
        return _size;
    }

    void setSize(const Vector2ui &size) noexcept
    {
        _size = size;
    }

    const std::string &getFormat() const noexcept
    {
        return _format;
    }

    void setFormat(std::string format) noexcept
    {
        _format = format;
    }

    uint32_t getQuality() const noexcept
    {
        return _quality;
    }

    void setQuality(uint32_t quality)
    {
        _quality = quality;
    }

private:
    Vector2ui _size{0u, 0u};
    std::string _format{"png"};
    uint32_t _quality{100u};
};

template<>
struct JsonAdapter<ImageSettings> : ObjectAdapter<ImageSettings>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ImageSettings");
        builder
            .getset(
                "format",
                [](auto &object) -> auto & { return object.getFormat(); },
                [](auto &object, auto value) { object.setFormat(std::move(value)); })
            .description("Image format (jpg or png)")
            .defaultValue("png");
        builder
            .getset(
                "quality",
                [](auto &object) { return object.getQuality(); },
                [](auto &object, auto value) { object.setQuality(value); })
            .description("Image quality (0 = lowest quality, 100 = highest quality")
            .defaultValue(100);
        builder
            .getset(
                "size",
                [](auto &object) -> auto & { return object.getSize(); },
                [](auto &object, const auto &value) { object.setSize(value); })
            .description("Image width and height")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns

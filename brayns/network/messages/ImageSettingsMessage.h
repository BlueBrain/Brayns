/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/FrameBuffer.h>

#include <brayns/json/JsonAdapterMacro.h>

#include <optional>

namespace brayns
{
class ImageSettings
{
public:
    ImageSettings() = default;
    ImageSettings(const Vector2ui &baseSize)
        : _size(baseSize)
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

    void setFormat(const std::string &format) noexcept
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

BRAYNS_JSON_ADAPTER_BEGIN(ImageSettings)
BRAYNS_JSON_ADAPTER_GETSET("format", getFormat, setFormat, "Image format (jpg or png)", Required(false))
BRAYNS_JSON_ADAPTER_GETSET(
    "quality",
    getQuality,
    setQuality,
    "Image quality (0 = lowest quality, 100 = highest quality",
    Required(false))
BRAYNS_JSON_ADAPTER_GETSET("size", getSize, setSize, "Image dimensions [width, height]", Required(false))
BRAYNS_JSON_ADAPTER_END()
}

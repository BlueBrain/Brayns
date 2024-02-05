/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
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

#include "Texture2D.h"

namespace brayns
{
Texture2D::Texture2D(const Type type_, const std::string& filename_,
                     const uint8_t channels_, const uint8_t depth_,
                     const uint32_t width_, const uint32_t height_)
    : type(type_)
    , filename(filename_)
    , channels(channels_)
    , depth(depth_)
    , width(width_)
    , height(height_)
{
    _rawData.resize(type == Type::cubemap ? 6 : 1);
    setMipLevels(1);
}

void Texture2D::setMipLevels(const uint8_t mips)
{
    if (mips == _mipLevels)
        return;
    _mipLevels = mips;
    for (auto& data : _rawData)
        data.resize(mips);
}

void Texture2D::setRawData(unsigned char* data, const size_t size,
                           const uint8_t face, const uint8_t mip)
{
    _rawData[face][mip].clear();
    _rawData[face][mip].assign(data, data + size);
}

void Texture2D::setRawData(std::vector<unsigned char>&& rawData,
                           const uint8_t face, const uint8_t mip)
{
    _rawData[face][mip] = std::move(rawData);
}

uint8_t Texture2D::getPossibleMipMapsLevels() const
{
    uint8_t mipMapLevels = 1u;
    auto nx = width;
    auto ny = height;
    while (nx % 2 == 0 && ny % 2 == 0)
    {
        nx /= 2;
        ny /= 2;
        ++mipMapLevels;
    }
    return mipMapLevels;
}
} // namespace brayns

/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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
Texture2D::Texture2D()
    : _nbChannels(0)
    , _depth(0)
    , _width(0)
    , _height(0)
{
    _rawData.resize(1);
}

void Texture2D::setRawData(unsigned char* data, size_t size, const size_t mip)
{
    _rawData[mip].clear();
    _rawData[mip].assign(data, data + size);
}

void Texture2D::setRawData(std::vector<unsigned char>&& rawData,
                           const size_t mip)
{
    _rawData[mip] = std::move(rawData);
}

uint8_t Texture2D::getPossibleMipMapsLevels() const
{
    uint8_t mipMapLevels = 1u;
    uint32_t nx = _width;
    uint32_t ny = _height;
    while (nx % 2 == 0 && ny % 2 == 0)
    {
        nx /= 2;
        ny /= 2;
        ++mipMapLevels;
    }
    return mipMapLevels;
}
}

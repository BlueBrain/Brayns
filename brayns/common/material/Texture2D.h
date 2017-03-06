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

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <vector>

namespace brayns
{
enum TextureType
{
    TT_DIFFUSE = 0,
    TT_NORMALS,
    TT_BUMP,
    TT_SPECULAR,
    TT_EMISSIVE,
    TT_OPACITY,
    TT_REFLECTION,
    TT_REFRACTION,
    TT_OCCLUSION
};

class Texture2D
{
public:
    BRAYNS_API Texture2D();

    BRAYNS_API TextureType getType() const { return _type; }
    BRAYNS_API void setType(TextureType value) { _type = value; }
    BRAYNS_API size_t getNbChannels() const { return _nbChannels; }
    BRAYNS_API void setNbChannels(size_t value) { _nbChannels = value; }
    BRAYNS_API size_t getDepth() const { return _depth; }
    BRAYNS_API void setDepth(size_t value) { _depth = value; }
    BRAYNS_API size_t getWidth() const { return _width; }
    BRAYNS_API void setWidth(size_t value) { _width = value; }
    BRAYNS_API size_t getHeight() const { return _height; }
    BRAYNS_API void setHeight(size_t value) { _height = value; }
    BRAYNS_API unsigned char* getRawData() { return _rawData.data(); }
    BRAYNS_API void setRawData(unsigned char* data, size_t size);

private:
    TextureType _type;                   // Diffuse, normal, bump, etc
    size_t _nbChannels;                  // Number of color channels per pixel
    size_t _depth;                       // Bytes per color channel
    size_t _width;                       // Pixels per row
    size_t _height;                      // Pixels per column
    std::vector<unsigned char> _rawData; // Binary texture raw data;
};
}

#endif // TEXTURE2D_H

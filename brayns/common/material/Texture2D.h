/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <vector>

namespace brayns
{
enum class TextureWrapMode
{
    clamp_to_border,
    clamp_to_edge,
    mirror,
    repeat
};

class Texture2D
{
public:
    enum class Type
    {
        default_,
        cubemap,
        normal_roughness
    };

    BRAYNS_API Texture2D(const Type type = Type::default_);

    const std::string& getFilename() const { return _filename; }
    void setFilename(const std::string& value) { _filename = value; }
    uint8_t getNbChannels() const { return _nbChannels; }
    void setNbChannels(const uint8_t value) { _nbChannels = value; }
    uint8_t getDepth() const { return _depth; }
    void setDepth(const uint8_t value) { _depth = value; }
    size_t getWidth() const { return _width; }
    void setWidth(const size_t value) { _width = value; }
    size_t getHeight() const { return _height; }
    void setHeight(const size_t value) { _height = value; }
    size_t getSizeInBytes() const
    {
        return _height * _width * _depth * _nbChannels;
    }
    void setMipLevels(const uint8_t mips);

    uint8_t getMipLevels() const { return _mipLevels; }
    template <typename T>
    const T* getRawData(const uint8_t face = 0, const uint8_t mip = 0) const
    {
        return reinterpret_cast<const T*>(_rawData[face][mip].data());
    }
    void setRawData(unsigned char* data, const size_t size,
                    const uint8_t face = 0, const uint8_t mip = 0);
    void setRawData(std::vector<unsigned char>&& rawData,
                    const uint8_t face = 0, const uint8_t mip = 0);

    uint8_t getPossibleMipMapsLevels() const;

    Type getType() const { return _type; }
    bool isCubeMap() const { return _type == Type::cubemap; }
    uint8_t getNumFaces() const { return isCubeMap() ? 6 : 1; }
    void setWrapMode(const TextureWrapMode mode) { _wrapMode = mode; }
    TextureWrapMode getWrapMode() const { return _wrapMode; }
private:
    const Type _type;
    std::string _filename;
    uint8_t _nbChannels{0};
    uint8_t _depth{0};
    size_t _width{0};
    size_t _height{0};
    size_t _mipLevels{0};
    TextureWrapMode _wrapMode{TextureWrapMode::mirror};
    // faces, mips, pixels
    std::vector<std::vector<std::vector<unsigned char>>> _rawData;
};
}

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

#pragma once

#include <cstdint>
#include <string>
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
        normal_roughness,
        aoe
    };

    Texture2D(const Type type, const std::string& filename,
              const uint8_t channels, const uint8_t depth, const uint32_t width,
              const uint32_t height);

    size_t getSizeInBytes() const { return height * width * depth * channels; }
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

    bool isCubeMap() const { return type == Type::cubemap; }
    bool isNormalMap() const { return type == Type::normal_roughness; }
    uint8_t getNumFaces() const { return isCubeMap() ? 6 : 1; }
    void setWrapMode(const TextureWrapMode mode) { _wrapMode = mode; }
    TextureWrapMode getWrapMode() const { return _wrapMode; }
    const Type type;
    const std::string filename;
    const uint8_t channels;
    const uint8_t depth;
    const uint32_t width;
    const uint32_t height;

private:
    uint8_t _mipLevels{0};
    TextureWrapMode _wrapMode{TextureWrapMode::repeat};
    // faces, mips, pixels
    std::vector<std::vector<std::vector<unsigned char>>> _rawData;
};
} // namespace brayns

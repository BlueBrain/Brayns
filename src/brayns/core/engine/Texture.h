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

#include <string>

#include "Data.h"
#include "Device.h"
#include "Object.h"
#include "TransferFunction.h"
#include "Volume.h"

namespace brayns
{
class Texture : public Managed<OSPTexture>
{
public:
    using Managed::Managed;
};

enum class TextureFormat
{
    Rgba8 = OSP_TEXTURE_RGBA8,
    Srgba8 = OSP_TEXTURE_SRGBA,
    Rgba32F = OSP_TEXTURE_RGBA32F,
    Rgb8 = OSP_TEXTURE_RGB8,
    Srgb8 = OSP_TEXTURE_SRGB,
    Rgb32F = OSP_TEXTURE_RGB32F,
    R8 = OSP_TEXTURE_R8,
    Ra8 = OSP_TEXTURE_RA8,
    L8 = OSP_TEXTURE_L8,
    La8 = OSP_TEXTURE_LA8,
    R32F = OSP_TEXTURE_R32F,
    Rgba16 = OSP_TEXTURE_RGBA16,
    Rgb16 = OSP_TEXTURE_RGB16,
    Ra16 = OSP_TEXTURE_RA16,
    R16 = OSP_TEXTURE_R16,
};

DataType getDataType(TextureFormat format);
std::size_t getSize(TextureFormat format);

enum class TextureFilter
{
    Linear = OSP_TEXTURE_FILTER_LINEAR,
    Nearest = OSP_TEXTURE_FILTER_NEAREST,
};

enum class TextureWrap
{
    Repeat = OSP_TEXTURE_WRAP_REPEAT,
    MirroredRepeat = OSP_TEXTURE_WRAP_MIRRORED_REPEAT,
    ClampToEdge = OSP_TEXTURE_WRAP_CLAMP_TO_EDGE,
};

struct TextureData2D
{
    std::string value;
    TextureFormat format;
    Size2 size;
};

struct Texture2DSettings
{
    TextureFilter filter = TextureFilter::Linear;
    TextureWrap wrap = TextureWrap::Repeat;
};

class Texture2D : public Texture
{
public:
    using Texture::Texture;

    void update(const Texture2DSettings &settings);
};

Texture2D createTexture2D(Device &device, const TextureData2D &data, const Texture2DSettings &settings = {});

struct VolumeTextureSettings
{
    Volume volume;
    TransferFunction transferFunction;
};

class VolumeTexture : public Texture
{
public:
    using Texture::Texture;

    void update(const VolumeTextureSettings &settings);
};

VolumeTexture createVolumeTexture(Device &device, const VolumeTextureSettings &settings);
}

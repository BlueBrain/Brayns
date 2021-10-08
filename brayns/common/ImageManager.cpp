/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include "ImageManager.h"
#include <brayns/common/log.h>
#include <brayns/common/utils/filesystem.h>
#include <brayns/common/utils/imageUtils.h>

namespace brayns
{
namespace
{
std::vector<unsigned char> getRawData(const freeimage::ImagePtr& image,
                                      const bool flip = true)
{
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    freeimage::SwapRedBlue32(image.get());
#endif

    const auto width = FreeImage_GetWidth(image.get());
    const auto height = FreeImage_GetHeight(image.get());
    const auto bpp = FreeImage_GetBPP(image.get());
    const auto pitch = width * bpp / 8;

    std::vector<unsigned char> rawData(height * pitch);
    FreeImage_ConvertToRawBits(rawData.data(), image.get(), pitch, bpp,
                               FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK,
                               FI_RGBA_BLUE_MASK, flip);
    return rawData;
}

void setRawData(Texture2DPtr texture, const freeimage::ImagePtr& image,
                const uint8_t mip = 0)
{
    auto width = texture->width;
    auto height = texture->height;
    for (uint8_t i = 0; i < mip; ++i)
    {
        width /= 2;
        height /= 2;
    }
    const bool flipFace = !texture->isCubeMap();
    for (uint8_t face = 0; face < texture->getNumFaces(); ++face)
    {
        const auto offset = face * width;
        freeimage::ImagePtr faceImg(FreeImage_CreateView(image.get(), offset, 0,
                                                         offset + width,
                                                         height));
        texture->setRawData(getRawData(faceImg, flipFace), face, mip);
    }
}
} // namespace

Texture2DPtr ImageManager::importTextureFromFile(
    const std::string& filename BRAYNS_UNUSED,
    const TextureType type BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_FREEIMAGE
    auto format = FreeImage_GetFileType(filename.c_str());
    if (format == FIF_UNKNOWN)
        format = FreeImage_GetFIFFromFilename(filename.c_str());
    if (format == FIF_UNKNOWN)
        return {};

    freeimage::ImagePtr image(FreeImage_Load(format, filename.c_str()));
    if (!image)
        return {};

    uint8_t depth = 1;
    switch (FreeImage_GetImageType(image.get()))
    {
    case FIT_BITMAP:
        depth = 1;
        break;
    case FIT_UINT16:
    case FIT_INT16:
    case FIT_RGB16:
        depth = 2;
        break;
    case FIT_UINT32:
    case FIT_INT32:
    case FIT_RGBA16:
    case FIT_FLOAT:
    case FIT_RGBF:
    case FIT_RGBAF:
        depth = 4;
        break;
    case FIT_DOUBLE:
    case FIT_COMPLEX:
        depth = 8;
        break;
    default:
        return {};
    }

    auto width = FreeImage_GetWidth(image.get());
    const auto height = FreeImage_GetHeight(image.get());
    const auto bytesPerPixel = FreeImage_GetBPP(image.get()) / 8;
    const auto channels = bytesPerPixel / depth;
    FreeImage_FlipVertical(image.get());

    Texture2D::Type textureType = Texture2D::Type::default_;
    const bool isCubeMap =
        type == TextureType::irradiance || type == TextureType::radiance;
    if (isCubeMap)
    {
        textureType = Texture2D::Type::cubemap;
        width /= 6;
    }
    else if (type == TextureType::normals) // TODO: only valid for PBR
        textureType = Texture2D::Type::normal_roughness;
    else if (type == TextureType::specular) // TODO: only valid for BBP
        textureType = Texture2D::Type::aoe;

    auto texture = std::make_shared<Texture2D>(textureType, filename, channels,
                                               depth, width, height);
    if (isCubeMap || type == TextureType::brdf_lut)
        texture->setWrapMode(TextureWrapMode::clamp_to_edge);

    setRawData(texture, image);

    const auto path = fs::path(filename).parent_path().string();
    const auto basename = path + "/" + fs::path(filename).stem().string();
    const auto ext = fs::path(filename).extension().string();

    uint8_t mipLevels = 1;
    while (fs::exists(basename + std::to_string((int)mipLevels) + ext))
        ++mipLevels;

    texture->setMipLevels(mipLevels);

    for (uint8_t mip = 1; mip < mipLevels; ++mip)
    {
        freeimage::ImagePtr mipImage(FreeImage_Load(
            format, (basename + std::to_string((int)mip) + ext).c_str()));
        FreeImage_FlipVertical(mipImage.get());

        setRawData(texture, mipImage, mip);
    }
    return texture;
#else
    BRAYNS_ERROR << "FreeImage is required to load images from file"
                 << std::endl;
    return {};
#endif
}
} // namespace brayns

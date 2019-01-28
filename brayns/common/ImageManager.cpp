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
#include <brayns/common/utils/imageUtils.h>

namespace brayns
{
Texture2DPtr ImageManager::importTextureFromFile(
    const std::string& filename BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_FREEIMAGE
    freeimage::ImagePtr image;
    if (auto temporary = FreeImage_Load(FreeImage_GetFileType(filename.c_str()),
                                        filename.c_str()))
    {
        image.reset(FreeImage_ConvertTo32Bits(temporary));
        FreeImage_Unload(temporary);
    }
    else
        return nullptr;

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    freeimage::SwapRedBlue32(image.get());
#endif

    const auto width = FreeImage_GetWidth(image.get());
    const auto height = FreeImage_GetHeight(image.get());
    const auto pitch = FreeImage_GetPitch(image.get());
    const auto channels = 4;

    std::vector<unsigned char> rawData(height * pitch);
    FreeImage_ConvertToRawBits(rawData.data(), image.get(), pitch, channels * 8,
                               FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK,
                               FI_RGBA_BLUE_MASK, TRUE);

    auto texture = std::make_shared<Texture2D>();
    texture->setFilename(filename);
    texture->setWidth(width);
    texture->setHeight(height);
    texture->setNbChannels(channels);
    texture->setDepth(1);
    texture->setRawData(std::move(rawData));
    return texture;
#else
    BRAYNS_ERROR << "FreeImage is required to load images from file"
                 << std::endl;
    return nullptr;
#endif
}
}

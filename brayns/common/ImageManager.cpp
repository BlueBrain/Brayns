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
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/utils/imageUtils.h>

namespace brayns
{
bool ImageManager::exportFrameBufferToFile(
    FrameBuffer& frameBuffer BRAYNS_UNUSED,
    const std::string& filename BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_FREEIMAGE
    auto image = freeimage::getImageFromFrameBuffer(frameBuffer);
    FreeImage_Save(FreeImage_GetFIFFromFilename(filename.c_str()), image.get(),
                   filename.c_str());

    return true;
#else
    BRAYNS_DEBUG << "FreeImage is required to export frames to file"
                 << std::endl;
    return false;
#endif
}

Texture2DPtr ImageManager::importTextureFromFile(
    const std::string& filename BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_FREEIMAGE
    freeimage::ImagePtr image;
    if (auto temporary = FreeImage_Load(FreeImage_GetFileType(filename.c_str()),
                                        filename.c_str()))
    {
        image.reset(FreeImage_IsTransparent(temporary)
                        ? FreeImage_ConvertTo32Bits(temporary)
                        : FreeImage_ConvertTo24Bits(temporary));
        FreeImage_Unload(temporary);
    }
    else
        return nullptr;

    FreeImage_FlipVertical(image.get());
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    freeimage::SwapRedBlue32(image.get());
#endif

    Texture2DPtr texture(new Texture2D);
    texture->setFilename(filename);
    texture->setWidth(FreeImage_GetWidth(image.get()));
    texture->setHeight(FreeImage_GetHeight(image.get()));
    texture->setNbChannels(FreeImage_IsTransparent(image.get()) ? 4 : 3);
    texture->setDepth(1);
    texture->setRawData((unsigned char*)FreeImage_GetBits(image.get()),
                        texture->getWidth() * texture->getHeight() *
                            texture->getNbChannels());

    BRAYNS_DEBUG << filename << ": " << texture->getWidth() << "x"
                 << texture->getHeight() << "x" << texture->getNbChannels()
                 << "x" << texture->getDepth() << " added to the texture cache"
                 << std::endl;
    return texture;
#else
    BRAYNS_DEBUG << "FreeImage is required to load images from file"
                 << std::endl;
    return nullptr;
#endif
}
}

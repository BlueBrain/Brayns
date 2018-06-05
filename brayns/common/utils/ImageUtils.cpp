/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "ImageUtils.h"
#include "base64/base64.h"

#include <brayns/common/renderer/FrameBuffer.h>

namespace
{
template <class T>
inline void INPLACESWAP(T& a, T& b)
{
    a ^= b;
    b ^= a;
    a ^= b;
}
}
namespace brayns
{
namespace freeimage
{
// https://github.com/imazen/freeimage/blob/master/Source/FreeImage/Conversion.cpp#L56
bool SwapRedBlue32(FIBITMAP* freeImage)
{
    if (FreeImage_GetImageType(freeImage) != FIT_BITMAP)
        return false;

    const unsigned bytesperpixel = FreeImage_GetBPP(freeImage) / 8;
    if (bytesperpixel > 4 || bytesperpixel < 3)
        return false;

    const unsigned height = FreeImage_GetHeight(freeImage);
    const unsigned pitch = FreeImage_GetPitch(freeImage);
    const unsigned lineSize = FreeImage_GetLine(freeImage);

    BYTE* line = FreeImage_GetBits(freeImage);
    for (unsigned y = 0; y < height; ++y, line += pitch)
        for (BYTE* pixel = line; pixel < line + lineSize;
             pixel += bytesperpixel)
        {
            INPLACESWAP(pixel[0], pixel[2]);
        }
    return true;
}

ImagePtr getImageFromFrameBuffer(FrameBuffer& frameBuffer)
{
    frameBuffer.map();
    const auto colorBuffer = frameBuffer.getColorBuffer();
    const auto& size = frameBuffer.getSize();

    ImagePtr image(FreeImage_ConvertFromRawBits(
        colorBuffer, size.x(), size.y(), frameBuffer.getColorDepth() * size.x(),
        8 * frameBuffer.getColorDepth(), 0xFF0000, 0x00FF00, 0x0000FF, false));

    frameBuffer.unmap();

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    SwapRedBlue32(image.get());
#endif
    return image;
}

std::string getBase64Image(FrameBuffer& frameBuffer, const std::string& format,
                           const int quality)
{
    FreeImage_SetOutputMessage(
        [](auto, auto message) { throw std::runtime_error(message); });

    auto image = freeimage::getImageFromFrameBuffer(frameBuffer);

    auto fif =
        format == "jpg" ? FIF_JPEG : FreeImage_GetFIFFromFormat(format.c_str());
    if (fif == FIF_JPEG)
        image.reset(FreeImage_ConvertTo24Bits(image.get()));
    else if (fif == FIF_UNKNOWN)
        throw std::runtime_error("Unknown format: " + format);

    int flags = quality;
    if (fif == FIF_TIFF)
        flags = TIFF_NONE;

    freeimage::MemoryPtr memory(FreeImage_OpenMemory());

    FreeImage_SaveToMemory(fif, image.get(), memory.get(), flags);

    BYTE* pixels = NULL;
    DWORD numPixels = 0;
    FreeImage_AcquireMemory(memory.get(), &pixels, &numPixels);
    return {base64_encode(pixels, numPixels)};
}
}
}

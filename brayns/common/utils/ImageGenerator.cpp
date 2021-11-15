/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "ImageGenerator.h"

#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/utils/base64/base64.h>

namespace brayns
{
ImageGenerator::~ImageGenerator()
{
    if (_compressor)
        tjDestroy(_compressor);
}

ImageGenerator::ImageBase64 ImageGenerator::createImage(
    freeimage::ImagePtr&& image, const std::string& format,
    const uint8_t quality)
{
    return {freeimage::getBase64Image(std::move(image), format, quality)};
}

ImageGenerator::ImageBase64 ImageGenerator::createImage(
    std::vector<freeimage::ImagePtr>& images, const std::string& format,
    const uint8_t quality)
{
    if (images.size() == 1)
        return createImage(std::move(images[0]), format, quality);

    return {freeimage::getBase64Image(freeimage::mergeImages(images), format,
                                      quality)};
}

ImageGenerator::ImageJPEG ImageGenerator::createJPEG(
    const uint8_t* colorBuffer, const FrameBufferFormat format,
    const Vector2ui& size, uint8_t quality)
{
    if (!colorBuffer)
        return ImageJPEG();

    int32_t pixelFormat = TJPF_RGBX;
    switch (format)
    {
    case FrameBufferFormat::bgra_i8:
        pixelFormat = TJPF_BGRX;
        break;
    case FrameBufferFormat::rgba_i8:
    default:
        pixelFormat = TJPF_RGBX;
    }

    ImageJPEG image;
    image.data = _encodeJpeg(size.x, size.y, colorBuffer, pixelFormat, quality,
                             image.size);
    return image;
}

ImageGenerator::ImageJPEG::JpegData ImageGenerator::_encodeJpeg(
    const uint32_t width, const uint32_t height, const uint8_t* rawData,
    const int32_t pixelFormat, const uint8_t quality, unsigned long& dataSize)
{
    uint8_t* tjSrcBuffer = const_cast<uint8_t*>(rawData);
    const int32_t color_components = 4; // Color Depth
    const int32_t tjPitch = width * color_components;
    const int32_t tjPixelFormat = pixelFormat;

    uint8_t* tjJpegBuf = 0;
    const int32_t tjJpegSubsamp = TJSAMP_444;
    const int32_t tjFlags = TJXOP_ROT180;

    const int32_t success =
        tjCompress2(_compressor, tjSrcBuffer, width, tjPitch, height,
                    tjPixelFormat, &tjJpegBuf, &dataSize, tjJpegSubsamp,
                    quality, tjFlags);

    if (success != 0)
    {
        Log::error("libjpeg-turbo image conversion failure.");
        return 0;
    }
    return ImageJPEG::JpegData{tjJpegBuf};
}
} // namespace brayns

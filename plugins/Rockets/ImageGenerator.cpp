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

#include "ImageGenerator.h"

#include <brayns/common/utils/base64/base64.h>
#include <brayns/common/utils/imageUtils.h>
#include <brayns/engineapi/FrameBuffer.h>
#include <brayns/parameters/ApplicationParameters.h>

namespace brayns
{
ImageGenerator::~ImageGenerator()
{
    if (_compressor)
        tjDestroy(_compressor);
}

ImageGenerator::ImageBase64 ImageGenerator::createImage(
    FrameBuffer& frameBuffer BRAYNS_UNUSED,
    const std::string& format BRAYNS_UNUSED,
    const uint8_t quality BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_FREEIMAGE
    return {freeimage::getBase64Image(frameBuffer.getImage(), format, quality)};
#else
    BRAYNS_WARN << "No FreeImage found, will take TurboJPEG snapshot; "
                << "ignoring format '" << format << "'" << std::endl;
    const auto& jpeg = createJPEG(frameBuffer, quality);
    return {base64_encode(jpeg.data.get(), jpeg.size)};
#endif
}

ImageGenerator::ImageBase64 ImageGenerator::createImage(
    const std::vector<FrameBufferPtr>& frameBuffers BRAYNS_UNUSED,
    const std::string& format BRAYNS_UNUSED,
    const uint8_t quality BRAYNS_UNUSED)
{
    if (frameBuffers.size() == 1)
        return createImage(*frameBuffers[0], format, quality);

#ifdef BRAYNS_USE_FREEIMAGE
    std::vector<freeimage::ImagePtr> images;
    for (auto frameBuffer : frameBuffers)
        images.push_back(frameBuffer->getImage());
    return {freeimage::getBase64Image(freeimage::mergeImages(images), format,
                                      quality)};
#else
    throw std::runtime_error("Need FreeImage; cannot create any image");
#endif
}

ImageGenerator::ImageJPEG ImageGenerator::createJPEG(
    FrameBuffer& frameBuffer BRAYNS_UNUSED, const uint8_t quality BRAYNS_UNUSED)
{
    frameBuffer.map();
    const auto colorBuffer = frameBuffer.getColorBuffer();
    if (!colorBuffer)
    {
        frameBuffer.unmap();
        return ImageJPEG();
    }

    int32_t pixelFormat = TJPF_RGBX;
    switch (frameBuffer.getFrameBufferFormat())
    {
    case FrameBufferFormat::bgra_i8:
        pixelFormat = TJPF_BGRX;
        break;
    case FrameBufferFormat::rgba_i8:
    default:
        pixelFormat = TJPF_RGBX;
    }

    const auto& frameSize = frameBuffer.getSize();
    ImageJPEG image;
    image.data = _encodeJpeg(frameSize.x, frameSize.y, colorBuffer, pixelFormat,
                             quality, image.size);
    frameBuffer.unmap();
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
        BRAYNS_ERROR << "libjpeg-turbo image conversion failure" << std::endl;
        return 0;
    }
    return ImageJPEG::JpegData{tjJpegBuf};
}
} // namespace brayns

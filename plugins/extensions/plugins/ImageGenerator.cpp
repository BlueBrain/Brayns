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

#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/parameters/ApplicationParameters.h>

#ifdef BRAYNS_USE_MAGICKPP
#include <Magick++.h>
#endif

#include "base64/base64.h"

namespace brayns
{
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

ImageGenerator::ImageBase64 ImageGenerator::createImage(
    FrameBuffer& frameBuffer, const std::string& format, const uint8_t quality)
{
#ifdef BRAYNS_USE_MAGICKPP
    try
    {
        frameBuffer.map();
        auto colorBuffer = frameBuffer.getColorBuffer();
        Magick::Image image(frameBuffer.getSize().x(),
                            frameBuffer.getSize().y(), "RGBA",
                            MagickCore::CharPixel, colorBuffer);
        image.magick(format);
        image.quality(quality);

        Magick::Blob blob;
        image.flip();
        image.write(&blob);
        frameBuffer.unmap();

        return {blob.base64()};
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
#else
    BRAYNS_WARN << "No assimp found, will take TurboJPEG snapshot; "
                << "ignoring format '" << format << "'" << std::endl;
    const auto& jpeg = createJPEG(frameBuffer, quality);
    return {base64_encode(jpeg.data.get(), jpeg.size)};
#endif
}

ImageGenerator::ImageJPEG ImageGenerator::createJPEG(FrameBuffer& frameBuffer,
                                                     const uint8_t quality)
{
    if (_processingImageJpeg)
        return ImageJPEG();

    _processingImageJpeg = true;

    frameBuffer.map();
    auto colorBuffer = frameBuffer.getColorBuffer();
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
    image.data = _encodeJpeg(frameSize.x(), frameSize.y(), colorBuffer,
                             pixelFormat, quality, image.size);
    frameBuffer.unmap();
    _processingImageJpeg = false;
    return image;
}
}

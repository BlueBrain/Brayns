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

namespace brayns
{
void ImageGenerator::_resizeImage(uint8_t* srcData, const Vector2i& srcSize,
                                  const Vector2i& dstSize, uint8_ts& dstData)
{
    dstData.reserve(dstSize.x() * dstSize.y());
    size_t x_ratio =
        static_cast<size_t>(((srcSize.x() << 16) / dstSize.x()) + 1);
    size_t y_ratio =
        static_cast<size_t>(((srcSize.y() << 16) / dstSize.y()) + 1);

    for (int y = 0; y < dstSize.y(); ++y)
    {
        for (int x = 0; x < dstSize.x(); ++x)
        {
            const size_t x2 = ((x * x_ratio) >> 16);
            const size_t y2 = ((y * y_ratio) >> 16);
            dstData[(y * dstSize.x()) + x] = srcData[(y2 * srcSize.x()) + x2];
        }
    }
}

ImageGenerator::ImageJPEG::JpegData ImageGenerator::_encodeJpeg(
    const uint32_t width, const uint32_t height, const uint8_t* rawData,
    const int32_t pixelFormat, unsigned long& dataSize)
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
                    _appParams.getJpegCompression(), tjFlags);

    if (success != 0)
    {
        BRAYNS_ERROR << "libjpeg-turbo image conversion failure" << std::endl;
        return 0;
    }
    return ImageJPEG::JpegData{tjJpegBuf};
}

ImageGenerator::ImageJPEG ImageGenerator::createJPEG(FrameBuffer& frameBuffer)
{
    if (_processingImageJpeg)
        return ImageJPEG();

    _processingImageJpeg = true;
    const auto& newFrameSize = _appParams.getJpegSize();
    if (newFrameSize.x() == 0 || newFrameSize.y() == 0)
    {
        BRAYNS_ERROR << "Encountered invalid size of image JPEG: "
                     << newFrameSize << std::endl;

        return ImageJPEG();
    }

    const auto& frameSize = frameBuffer.getSize();
    auto colorBuffer = frameBuffer.getColorBuffer();
    if (!colorBuffer)
        return ImageJPEG();

    auto resizedColorBuffer = colorBuffer;

    uint8_ts resizedBuffer;
    if (frameSize != newFrameSize)
    {
        _resizeImage(colorBuffer, frameSize, newFrameSize, resizedBuffer);
        resizedColorBuffer = resizedBuffer.data();
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

    ImageJPEG image;
    image.data = _encodeJpeg(newFrameSize.x(), newFrameSize.y(),
                             resizedColorBuffer, pixelFormat, image.size);
    _processingImageJpeg = false;
    return image;
}
}

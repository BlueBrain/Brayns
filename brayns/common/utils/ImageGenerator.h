/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/types.h>

#include <turbojpeg.h>

namespace brayns
{
/**
 * A class which creates images for network communication from a FrameBuffer.
 */
class ImageGenerator
{
public:
    ImageGenerator() = default;
    ~ImageGenerator();

    struct ImageBase64
    {
        std::string data;
    };

    /**
     * Create a base64-encoded image from the given framebuffer in a specified
     * image format and quality.
     *
     * @param frameBuffer the framebuffer to use for getting the pixels
     * @param format FreeImage format string, or JPEG if FreeImage is not
     *               available
     * @param quality image format specific quality number
     * @return base64-encoded image
     * @throw std::runtime_error if image conversion failed or neither FreeImage
     *                           nor TurboJPEG is available
     */
    ImageBase64 createImage(FrameBuffer& frameBuffer, const std::string& format,
                            uint8_t quality);
    ImageBase64 createImage(const std::vector<FrameBufferPtr>& frameBuffers,
                            const std::string& format, uint8_t quality);

    struct ImageJPEG
    {
        struct tjDeleter
        {
            void operator()(uint8_t* ptr) { tjFree(ptr); }
        };
        using JpegData = std::unique_ptr<uint8_t, tjDeleter>;
        JpegData data;
        unsigned long size{0};
    };

    /**
     * Create a JPEG image from the given framebuffer in a specified quality.
     *
     * @param frameBuffer the framebuffer to use for getting the pixels
     * @param quality 1..100 JPEG quality
     * @return JPEG image with a size > 0 if valid, size == 0 on error.
     */
    ImageJPEG createJPEG(FrameBuffer& frameBuffer, uint8_t quality);

private:
    tjhandle _compressor{tjInitCompress()};

    ImageJPEG::JpegData _encodeJpeg(uint32_t width, uint32_t height,
                                    const uint8_t* rawData, int32_t pixelFormat,
                                    uint8_t quality, unsigned long& dataSize);
};
} // namespace brayns

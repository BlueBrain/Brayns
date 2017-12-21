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

#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <brayns/common/types.h>
#include <turbojpeg.h>

namespace brayns
{
class ImageGenerator
{
public:
    ImageGenerator(const ApplicationParameters& appParams)
        : _appParams(appParams)
    {
    }

    ~ImageGenerator()
    {
        if (_compressor)
            tjDestroy(_compressor);
    }

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

    ImageJPEG createJPEG(FrameBuffer& frameBuffer);

private:
    bool _processingImageJpeg = false;
    tjhandle _compressor{tjInitCompress()};
    const ApplicationParameters& _appParams;

    void _resizeImage(uint8_t* srcData, const Vector2i& srcSize,
                      const Vector2i& dstSize, uint8_ts& dstData);
    ImageJPEG::JpegData _encodeJpeg(const uint32_t width, const uint32_t height,
                                    const uint8_t* rawData,
                                    const int32_t pixelFormat,
                                    unsigned long& dataSize);
};
}

#endif

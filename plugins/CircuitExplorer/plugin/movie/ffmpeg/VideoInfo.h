/* Copyright (c) 2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
}

#include "Exception.h"

namespace ffmpeg
{
struct VideoInfo
{
    AVCodec* codec = nullptr;
    int width = 0;
    int height = 0;
    AVPixelFormat format = AV_PIX_FMT_NONE;
    int64_t bitrate = 0;
    int framerate = 0;

    static AVCodec* getCodec(const std::string& name)
    {
        auto codec = avcodec_find_encoder_by_name(name.c_str());
        if (!codec)
        {
            throw Exception("Invalid codec '" + name + "'");
        }
        return codec;
    }

    static AVPixelFormat getFormat(const std::string& name)
    {
        auto format = av_get_pix_fmt(name.c_str());
        if (format == AV_PIX_FMT_NONE)
        {
            throw Exception("Invalid pixel format: '" + name + "'");
        }
        return format;
    }
};
} // namespace ffmpeg
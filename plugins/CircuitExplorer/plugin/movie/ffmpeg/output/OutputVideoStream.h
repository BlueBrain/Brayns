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
#include <libavformat/avformat.h>
}

#include "../Exception.h"
#include "../VideoInfo.h"

namespace ffmpeg
{
class OutputVideoStream
{
public:
    static AVStream* add(AVFormatContext* context, const VideoInfo& info)
    {
        auto stream = _addStream(context, info.codec);
        _setupParameters(stream, info);
        return stream;
    }

private:
    static AVStream* _addStream(AVFormatContext* context, AVCodec* codec)
    {
        auto stream = avformat_new_stream(context, codec);
        if (!stream)
        {
            throw Exception("Cannot create output video stream");
        }
        return stream;
    }

    static void _setupParameters(AVStream* stream, const VideoInfo& info)
    {
        stream->codecpar->codec_id = info.codec->id;
        stream->codecpar->codec_type = info.codec->type;
        stream->codecpar->width = info.width;
        stream->codecpar->height = info.height;
        stream->codecpar->format = info.format;
        stream->codecpar->bit_rate = info.bitrate;
        stream->time_base = {1, info.framerate};
    }
};
} // namespace ffmpeg